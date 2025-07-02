#include "BMPVolumeViewer.h"
#include "ui_BMPVolumeViewer.h"

#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include <QDir>
#include <QFileInfo>
#include <QProgressDialog>

#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkAutoInit.h> // 添加VTK模块初始化宏

// 明确初始化所需的VTK模块
VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkInteractionStyle);
VTK_MODULE_INIT(vtkRenderingVolumeOpenGL2);
VTK_MODULE_INIT(vtkRenderingFreeType); // 可选，解决字体渲染问题

BMPVolumeViewer::BMPVolumeViewer(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::BMPVolumeViewer)
{
	ui->setupUi(this);

	// 确保VTK部件已正确创建
	if (!ui->vtkWidget) {
		QMessageBox::critical(this, "初始化错误", "VTK部件初始化失败!");
		return;
	}

	// 获取渲染窗口
	vtkRenderWindow* renderWindow = ui->vtkWidget->GetRenderWindow();
	if (!renderWindow) {
		QMessageBox::critical(this, "初始化错误", "无法获取VTK渲染窗口!");
		return;
	}

	// 创建渲染器
	renderer = vtkSmartPointer<vtkRenderer>::New();
	renderWindow->AddRenderer(renderer);

	// 初始化颜色和不透明度传输函数
	colorTransferFunction = vtkSmartPointer<vtkColorTransferFunction>::New();
	opacityTransferFunction = vtkSmartPointer<vtkPiecewiseFunction>::New();

	// 连接控制信号
	connect(ui->opacitySlider, &QSlider::valueChanged, this, &BMPVolumeViewer::on_opacitySlider_valueChanged);
	connect(ui->colorSlider, &QSlider::valueChanged, this, &BMPVolumeViewer::on_colorSlider_valueChanged);

	// 初始化默认值
	ui->filePatternEdit->setText("%s%d.00um.bmp");

	// 强制初始渲染以建立OpenGL上下文
	renderWindow->Render();
}

BMPVolumeViewer::~BMPVolumeViewer()
{
	// 清理资源
	if (ui->vtkWidget && ui->vtkWidget->GetRenderWindow()) {
		ui->vtkWidget->GetRenderWindow()->Finalize();
	}
	delete ui;
}

void BMPVolumeViewer::on_browseButton_clicked()
{
	QString directory = QFileDialog::getExistingDirectory(
		this,
		tr("选择BMP序列目录"),
		QDir::homePath(),
		QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
	);

	if (!directory.isEmpty()) {
		// 确保目录路径以分隔符结尾
		QDir dir(directory);
		ui->filePrefixEdit->setText(dir.filePath("depth_"));
	}
}

void BMPVolumeViewer::on_loadButton_clicked()
{
	QString filePrefix = ui->filePrefixEdit->text();
	QString filePattern = ui->filePatternEdit->text();

	if (filePrefix.isEmpty()) {
		QMessageBox::warning(this, "输入错误", "文件前缀不能为空!");
		return;
	}

	// 检查文件是否存在
	QFileInfo testFile((filePrefix + "0.00um.bmp").arg(0));
	if (!testFile.exists()) {
		QMessageBox::warning(this, "error",
			QString("could not find the files: %1\nplease check the filePrefix and filePattern.")
			.arg(testFile.filePath()));
		return;
	}

	// 创建BMP读取器
	bmpReader = vtkSmartPointer<vtkBMPReader>::New();
	bmpReader->SetFilePrefix(filePrefix.toStdString().c_str());
	bmpReader->SetFilePattern(filePattern.toStdString().c_str());
	bmpReader->SetDataExtent(0, 511, 0, 511, 0, 100);
	bmpReader->SetDataSpacing(1, 1, 1);
	bmpReader->SetDataOrigin(0, 0, 0);
	bmpReader->SetAllow8BitBMP(true);

	try {
		// 显示进度对话框
		QProgressDialog progress("loading BMP sequence...", "quit", 0, 0, this);
		progress.setWindowModality(Qt::WindowModal);
		progress.show();
		QApplication::processEvents();

		// 更新读取器
		bmpReader->Update();
		progress.close();

		// 检查数据是否成功读取
		vtkImageData* imageData = bmpReader->GetOutput();
		if (!imageData || imageData->GetPointData()->GetScalars() == nullptr) {
			QMessageBox::critical(this, "数据错误",
				"未读取到图像数据!\n请检查文件路径和格式。");
			return;
		}

		// 获取实际数据范围
		int* dims = imageData->GetDimensions();
		qDebug() << "size of image: " << dims[0] << "x" << dims[1] << "x" << dims[2];

		setupVTKPipeline();

	}
	catch (std::exception& e) {
		QMessageBox::critical(this, "加载错误",
			QString("加载BMP序列时发生错误:\n%1").arg(e.what()));
	}
	catch (...) {
		QMessageBox::critical(this, "加载错误", "加载BMP序列时发生未知错误!");
	}
}

void BMPVolumeViewer::setupVTKPipeline()
{
	if (!renderer) {
		QMessageBox::critical(this, "渲染错误", "渲染器未初始化!");
		return;
	}

	// 清理之前的渲染
	renderer->RemoveAllViewProps();

	// 创建降采样过滤器
	resample = vtkSmartPointer<vtkImageResample>::New();
	resample->SetInputConnection(bmpReader->GetOutputPort());
	resample->SetAxisMagnificationFactor(0, 0.5);
	resample->SetAxisMagnificationFactor(1, 0.5);
	resample->SetAxisMagnificationFactor(2, 1.0);

	// 初始化传输函数
	updateColorTransferFunction(ui->colorSlider->value() / 100.0);
	updateOpacityTransferFunction(ui->opacitySlider->value() / 100.0);

	// 设置体属性
	volumeProperty = vtkSmartPointer<vtkVolumeProperty>::New();
	volumeProperty->SetColor(colorTransferFunction);
	volumeProperty->SetScalarOpacity(opacityTransferFunction);
	volumeProperty->SetInterpolationTypeToLinear();
	volumeProperty->ShadeOn();
	volumeProperty->SetAmbient(0.4);
	volumeProperty->SetDiffuse(0.6);
	volumeProperty->SetSpecular(0.2);

	// 创建体映射器
	volumeMapper = vtkSmartPointer<vtkFixedPointVolumeRayCastMapper>::New();
	volumeMapper->SetInputConnection(resample->GetOutputPort());

	// 创建体
	volume = vtkSmartPointer<vtkVolume>::New();
	volume->SetMapper(volumeMapper);
	volume->SetProperty(volumeProperty);

	// 添加到渲染器
	renderer->AddVolume(volume);
	renderer->SetBackground(0.2, 0.3, 0.4);
	renderer->ResetCamera();

	// 获取并初始化交互器
	vtkRenderWindow* renderWindow = ui->vtkWidget->GetRenderWindow();
	if (!renderWindow) {
		QMessageBox::critical(this, "渲染错误", "无法获取渲染窗口!");
		return;
	}

	vtkRenderWindowInteractor* interactor = renderWindow->GetInteractor();
	if (!interactor) {
		interactor = vtkRenderWindowInteractor::New();
		interactor->SetRenderWindow(renderWindow);
		renderWindow->SetInteractor(interactor);
		interactor->Delete(); // VTK会管理这个指针
	}

	// 设置交互样式
	vtkSmartPointer<vtkInteractorStyleTrackballCamera> style =
		vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
	interactor->SetInteractorStyle(style);

	// 初始化交互器
	if (!interactor->GetInitialized()) {
		interactor->Initialize();
	}

	// 更新渲染
	renderWindow->Render();
}

void BMPVolumeViewer::updateColorTransferFunction(double hue)
{
	if (!colorTransferFunction) return;

	colorTransferFunction->RemoveAllPoints();
	colorTransferFunction->AddRGBPoint(0, 0.0, 0.0, 0.0);
	colorTransferFunction->AddRGBPoint(64, 1.0, hue, 0.0);
	colorTransferFunction->AddRGBPoint(128, 1.0, 1.0, hue);
	colorTransferFunction->AddRGBPoint(192, hue, 1.0, 0.0);
	colorTransferFunction->AddRGBPoint(255, 0.0, hue, 1.0);
}

void BMPVolumeViewer::updateOpacityTransferFunction(double maxOpacity)
{
	if (!opacityTransferFunction) return;

	opacityTransferFunction->RemoveAllPoints();
	opacityTransferFunction->AddPoint(0, 0.0);
	opacityTransferFunction->AddPoint(50, 0.1 * maxOpacity);
	opacityTransferFunction->AddPoint(100, 0.3 * maxOpacity);
	opacityTransferFunction->AddPoint(150, 0.6 * maxOpacity);
	opacityTransferFunction->AddPoint(200, 0.9 * maxOpacity);
	opacityTransferFunction->AddPoint(255, 1.0 * maxOpacity);
}

void BMPVolumeViewer::on_opacitySlider_valueChanged(int value)
{
	if (!volumeProperty || !opacityTransferFunction) return;

	updateOpacityTransferFunction(value / 100.0);
	volumeProperty->SetScalarOpacity(opacityTransferFunction);

	if (ui->vtkWidget && ui->vtkWidget->GetRenderWindow()) {
		ui->vtkWidget->GetRenderWindow()->Render();
	}
}

void BMPVolumeViewer::on_colorSlider_valueChanged(int value)
{
	if (!volumeProperty || !colorTransferFunction) return;

	updateColorTransferFunction(value / 100.0);
	volumeProperty->SetColor(colorTransferFunction);

	if (ui->vtkWidget && ui->vtkWidget->GetRenderWindow()) {
		ui->vtkWidget->GetRenderWindow()->Render();
	}
}

void BMPVolumeViewer::on_resetViewButton_clicked()
{
	if (renderer && ui->vtkWidget && ui->vtkWidget->GetRenderWindow()) {
		renderer->ResetCamera();
		ui->vtkWidget->GetRenderWindow()->Render();
	}
}