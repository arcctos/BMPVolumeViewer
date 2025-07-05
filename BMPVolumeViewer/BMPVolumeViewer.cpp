#include "BMPVolumeViewer.h"
#include "ui_BMPVolumeViewer.h"

#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>
#include <QDir>
#include <QFileInfo>
#include <QProgressDialog>

// 包含必要的VTK头文件
#include <vtkImagePlaneWidget.h>
#include <vtkLookupTable.h>
#include <vtkImageMapToColors.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkAutoInit.h> // 添加VTK模块初始化宏

VTK_MODULE_INIT(vtkRenderingOpenGL2);
VTK_MODULE_INIT(vtkInteractionStyle);
VTK_MODULE_INIT(vtkRenderingVolumeOpenGL2);
VTK_MODULE_INIT(vtkRenderingFreeType);

BMPVolumeViewer::BMPVolumeViewer(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::BMPVolumeViewer)
{
	ui->setupUi(this);

	// 设置窗口最小尺寸以确保布局正确
	this->setMinimumSize(1200, 800);

	// 设置布局比例
	// 左侧三维窗口占6份，右侧切片窗口占4份
	ui->horizontalLayout_2->setStretch(0, 6);
	ui->horizontalLayout_2->setStretch(1, 4);

	// 切片视图垂直布局中两个切片各占1份
	ui->verticalLayout_3->setStretch(0, 1);
	ui->verticalLayout_3->setStretch(1, 1);

	// 初始化数据
	std::fill(std::begin(dataDimensions), std::end(dataDimensions), 0);
	std::fill(std::begin(dataSpacing), std::end(dataSpacing), 1.0);

	// 确保VTK部件已正确创建
	if (!ui->volumeWidget || !ui->xSliceWidget || !ui->ySliceWidget) {
		QMessageBox::critical(this, QStringLiteral("初始化错误"), QStringLiteral("VTK部件初始化失败!"));
		return;
	}

	// 获取渲染窗口
	vtkRenderWindow* volumeWindow = ui->volumeWidget->GetRenderWindow();
	vtkRenderWindow* xSliceWindow = ui->xSliceWidget->GetRenderWindow();
	vtkRenderWindow* ySliceWindow = ui->ySliceWidget->GetRenderWindow();

	if (!volumeWindow || !xSliceWindow || !ySliceWindow) {
		QMessageBox::critical(this, QStringLiteral("初始化错误"), QStringLiteral("无法获取VTK渲染窗口!"));
		return;
	}

	// 创建渲染器
	volumeRenderer = vtkSmartPointer<vtkRenderer>::New();
	xSliceRenderer = vtkSmartPointer<vtkRenderer>::New();
	ySliceRenderer = vtkSmartPointer<vtkRenderer>::New();

	volumeWindow->AddRenderer(volumeRenderer);
	xSliceWindow->AddRenderer(xSliceRenderer);
	ySliceWindow->AddRenderer(ySliceRenderer);

	// 初始化颜色和不透明度传输函数
	colorTransferFunction = vtkSmartPointer<vtkColorTransferFunction>::New();
	opacityTransferFunction = vtkSmartPointer<vtkPiecewiseFunction>::New();

	// 连接控制信号
	connect(ui->opacitySlider, &QSlider::valueChanged, this, &BMPVolumeViewer::on_opacitySlider_valueChanged);
	connect(ui->colorSlider, &QSlider::valueChanged, this, &BMPVolumeViewer::on_colorSlider_valueChanged);
	connect(ui->xSliceSlider, &QSlider::valueChanged, this, &BMPVolumeViewer::on_xSliceSlider_valueChanged);
	connect(ui->ySliceSlider, &QSlider::valueChanged, this, &BMPVolumeViewer::on_ySliceSlider_valueChanged);

	// 初始化默认值
	ui->filePatternEdit->setText("%s%d.00um.bmp");

	// 设置初始滑块值
	ui->xSliceSlider->setValue(50);
	ui->ySliceSlider->setValue(50);

	// 强制初始渲染以建立OpenGL上下文
	volumeWindow->Render();
	xSliceWindow->Render();
	ySliceWindow->Render();
}

BMPVolumeViewer::~BMPVolumeViewer()
{
	if (ui->volumeWidget && ui->volumeWidget->GetRenderWindow()) {
		ui->volumeWidget->GetRenderWindow()->Finalize();
	}
	if (ui->xSliceWidget && ui->xSliceWidget->GetRenderWindow()) {
		ui->xSliceWidget->GetRenderWindow()->Finalize();
	}
	if (ui->ySliceWidget && ui->ySliceWidget->GetRenderWindow()) {
		ui->ySliceWidget->GetRenderWindow()->Finalize();
	}
	delete ui;
}

// 设置坐标轴控件
void BMPVolumeViewer::setupAxesWidget()
{
	if (!ui->volumeWidget || !ui->volumeWidget->GetRenderWindow()) return;

	// 创建坐标轴
	vtkSmartPointer<vtkAxesActor> axes = vtkSmartPointer<vtkAxesActor>::New();
	axes->SetTotalLength(100, 100, 100); // 设置坐标轴长度

	// 创建方向标记部件
	axesWidget = vtkSmartPointer<vtkOrientationMarkerWidget>::New();
	axesWidget->SetOutlineColor(0.9300, 0.5700, 0.1300);
	axesWidget->SetOrientationMarker(axes);
	axesWidget->SetInteractor(ui->volumeWidget->GetRenderWindow()->GetInteractor());
	axesWidget->SetViewport(0.0, 0.0, 0.2, 0.2); // 设置在窗口中的位置和大小
	axesWidget->SetEnabled(1);
	axesWidget->InteractiveOn();
}

// 创建从颜色传输函数生成的查找表
vtkSmartPointer<vtkLookupTable> BMPVolumeViewer::createLookupTableFromColorTF()
{
	vtkSmartPointer<vtkLookupTable> lut = vtkSmartPointer<vtkLookupTable>::New();
	lut->SetNumberOfTableValues(256);
	lut->Build();

	for (int i = 0; i < 256; i++) {
		double rgb[3];
		colorTransferFunction->GetColor(static_cast<double>(i), rgb);
		lut->SetTableValue(i, rgb[0], rgb[1], rgb[2], 1.0);
	}

	return lut;
}

// 更新切片平面的查找表
void BMPVolumeViewer::updateSliceLookupTables()
{
	vtkSmartPointer<vtkLookupTable> lut = createLookupTableFromColorTF();

	if (xSlicePlane) {
		xSlicePlane->SetLookupTable(lut);
	}
	if (ySlicePlane) {
		ySlicePlane->SetLookupTable(lut);
	}
}

void BMPVolumeViewer::on_browseButton_clicked()
{
	QString directory = QFileDialog::getExistingDirectory(
		this,
		tr("select directory"),
		QDir::homePath(),
		QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks
	);

	if (!directory.isEmpty()) {
		QDir dir(directory);
		ui->filePrefixEdit->setText(dir.filePath("depth_"));
	}
}

void BMPVolumeViewer::on_loadButton_clicked()
{
	QString filePrefix = ui->filePrefixEdit->text();
	QString filePattern = ui->filePatternEdit->text();

	if (filePrefix.isEmpty()) {
		QMessageBox::warning(this, QStringLiteral("输入错误"), QStringLiteral("文件前缀不能为空!"));
		return;
	}

	// 检查文件是否存在
	QFileInfo testFile((filePrefix + "0.00um.bmp").arg(0));
	if (!testFile.exists()) {
		QMessageBox::warning(this, QStringLiteral("文件错误"),
			QStringLiteral("找不到测试文件: %1\n请检查文件前缀和格式是否正确。")
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
		QProgressDialog progress(QStringLiteral("加载BMP序列..."), QStringLiteral("取消"), 0, 0, this);
		progress.setWindowModality(Qt::WindowModal);
		progress.show();
		QApplication::processEvents();

		bmpReader->Update();
		progress.close();

		vtkImageData* imageData = bmpReader->GetOutput();
		if (!imageData || imageData->GetPointData()->GetScalars() == nullptr) {
			QMessageBox::critical(this, QStringLiteral("数据错误"),
				QStringLiteral("未读取到图像数据!\n请检查文件路径和格式。"));
			return;
		}

		// 获取数据信息
		imageData->GetDimensions(dataDimensions);
		imageData->GetSpacing(dataSpacing);

		qDebug() << QStringLiteral("加载的图像尺寸: ")
			<< dataDimensions[0] << "x"
			<< dataDimensions[1] << "x"
			<< dataDimensions[2];
		qDebug() << QStringLiteral("数据间距: ")
			<< dataSpacing[0] << ", "
			<< dataSpacing[1] << ", "
			<< dataSpacing[2];

		// 设置滑块范围
		ui->xSliceSlider->setRange(0, dataDimensions[0] - 1);
		ui->ySliceSlider->setRange(0, dataDimensions[1] - 1);
		ui->xSliceSlider->setValue(dataDimensions[0] / 2);
		ui->ySliceSlider->setValue(dataDimensions[1] / 2);

		setupVTKPipeline();
		setupSliceViews(); // 设置切片视图

		// 更新标签
		ui->xSliceLabel->setText(QStringLiteral("X切片位置: %1").arg(dataDimensions[0] / 2));
		ui->ySliceLabel->setText(QStringLiteral("Y切片位置: %1").arg(dataDimensions[1] / 2));
	}
	catch (std::exception& e) {
		QMessageBox::critical(this, QStringLiteral("加载错误"),
			QStringLiteral("加载BMP序列时发生错误:\n%1").arg(e.what()));
	}
	catch (...) {
		QMessageBox::critical(this, QStringLiteral("加载错误"), QStringLiteral("加载BMP序列时发生未知错误!"));
	}
}

void BMPVolumeViewer::setupVTKPipeline()
{
	if (!volumeRenderer) {
		QMessageBox::critical(this, QStringLiteral("加载BMP序列时发生未知错误!"), QStringLiteral("渲染器未初始化!"));
		return;
	}

	// 清理之前的渲染
	volumeRenderer->RemoveAllViewProps();

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
	volumeMapper->SetCropping(false);
	volumeMapper->SetSampleDistance(0.5);
	volumeMapper->SetImageSampleDistance(1.0);

	// 创建体
	volume = vtkSmartPointer<vtkVolume>::New();
	volume->SetMapper(volumeMapper);
	volume->SetProperty(volumeProperty);

	// 添加到渲染器
	volumeRenderer->AddVolume(volume);
	volumeRenderer->SetBackground(0.2, 0.3, 0.4);
	volumeRenderer->ResetCamera();

	// 获取并初始化交互器
	vtkRenderWindow* volumeWindow = ui->volumeWidget->GetRenderWindow();
	if (!volumeWindow) {
		QMessageBox::critical(this, QStringLiteral("渲染错误"), QStringLiteral("无法获取渲染窗口!"));
		return;
	}

	vtkRenderWindowInteractor* interactor = volumeWindow->GetInteractor();
	if (!interactor) {
		interactor = vtkRenderWindowInteractor::New();
		interactor->SetRenderWindow(volumeWindow);
		volumeWindow->SetInteractor(interactor);
		interactor->Delete();
	}

	// 设置交互样式
	vtkSmartPointer<vtkInteractorStyleTrackballCamera> style =
		vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
	interactor->SetInteractorStyle(style);

	// 初始化交互器
	if (!interactor->GetInitialized()) {
		interactor->Initialize();
	}

	// 设置坐标轴控件
	setupAxesWidget();

	// 更新渲染
	volumeWindow->Render();
}

// 设置切片视图
void BMPVolumeViewer::setupSliceViews()
{
	vtkImageData* imageData = bmpReader->GetOutput();
	if (!imageData) return;

	// 设置X切片视图
	vtkRenderWindow* xSliceWindow = ui->xSliceWidget->GetRenderWindow();
	if (!xSliceWindow) return;

	// 创建X切片平面控件
	xSlicePlane = vtkSmartPointer<vtkImagePlaneWidget>::New();
	xSlicePlane->SetInteractor(xSliceWindow->GetInteractor());
	xSlicePlane->SetInputData(imageData);
	xSlicePlane->SetPlaneOrientationToXAxes(); // 平行于X轴
	xSlicePlane->SetSliceIndex(dataDimensions[0] / 2); // 初始位置在中间
	xSlicePlane->DisplayTextOn();
	xSlicePlane->SetRightButtonAction(0); // 禁用右键操作
	xSlicePlane->SetMiddleButtonAction(0); // 禁用中键操作
	xSlicePlane->TextureInterpolateOff();
	xSlicePlane->SetResliceInterpolateToNearestNeighbour(); // 最近邻插值
	xSlicePlane->RestrictPlaneToVolumeOn();

	// 设置颜色查找表
	updateSliceLookupTables();

	// 设置平面属性
	xSlicePlane->GetPlaneProperty()->SetColor(1, 0, 0); // 红色边框

	xSlicePlane->On();

	// 设置X切片渲染器背景
	xSliceRenderer->SetBackground(0.1, 0.1, 0.1);
	xSliceRenderer->ResetCamera();

	// 设置Y切片视图
	vtkRenderWindow* ySliceWindow = ui->ySliceWidget->GetRenderWindow();
	if (!ySliceWindow) return;

	// 创建Y切片平面控件
	ySlicePlane = vtkSmartPointer<vtkImagePlaneWidget>::New();
	ySlicePlane->SetInteractor(ySliceWindow->GetInteractor());
	ySlicePlane->SetInputData(imageData);
	ySlicePlane->SetPlaneOrientationToYAxes(); // 平行于Y轴
	ySlicePlane->SetSliceIndex(dataDimensions[1] / 2); // 初始位置在中间
	ySlicePlane->DisplayTextOn();
	ySlicePlane->SetRightButtonAction(0); // 禁用右键操作
	ySlicePlane->SetMiddleButtonAction(0); // 禁用中键操作
	ySlicePlane->TextureInterpolateOff();
	ySlicePlane->SetResliceInterpolateToNearestNeighbour(); // 最近邻插值
	ySlicePlane->RestrictPlaneToVolumeOn();

	// 设置颜色查找表
	updateSliceLookupTables();

	// 设置平面属性
	ySlicePlane->GetPlaneProperty()->SetColor(0, 1, 0); // 绿色边框

	ySlicePlane->On();

	// 设置Y切片渲染器背景
	ySliceRenderer->SetBackground(0.1, 0.1, 0.1);
	ySliceRenderer->ResetCamera();

	// 更新渲染
	xSliceWindow->Render();
	ySliceWindow->Render();
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

	// 更新切片视图的颜色查找表
	updateSliceLookupTables();

	// 请求渲染更新
	if (ui->volumeWidget && ui->volumeWidget->GetRenderWindow()) {
		ui->volumeWidget->GetRenderWindow()->Render();
	}
	if (ui->xSliceWidget && ui->xSliceWidget->GetRenderWindow()) {
		ui->xSliceWidget->GetRenderWindow()->Render();
	}
	if (ui->ySliceWidget && ui->ySliceWidget->GetRenderWindow()) {
		ui->ySliceWidget->GetRenderWindow()->Render();
	}
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

	if (ui->volumeWidget && ui->volumeWidget->GetRenderWindow()) {
		ui->volumeWidget->GetRenderWindow()->Render();
	}
}

void BMPVolumeViewer::on_colorSlider_valueChanged(int value)
{
	if (!volumeProperty || !colorTransferFunction) return;

	updateColorTransferFunction(value / 100.0);
}

void BMPVolumeViewer::on_resetViewButton_clicked()
{
	if (volumeRenderer && ui->volumeWidget && ui->volumeWidget->GetRenderWindow()) {
		volumeRenderer->ResetCamera();
		ui->volumeWidget->GetRenderWindow()->Render();
	}
}

// X方向切片滑块变化
void BMPVolumeViewer::on_xSliceSlider_valueChanged(int value)
{
	if (xSlicePlane && dataDimensions[0] > 0) {
		xSlicePlane->SetSliceIndex(value);
		ui->xSliceLabel->setText(QStringLiteral("X切片位置: %1").arg(value));
		ui->xSliceWidget->GetRenderWindow()->Render();
	}
}

// Y方向切片滑块变化
void BMPVolumeViewer::on_ySliceSlider_valueChanged(int value)
{
	if (ySlicePlane && dataDimensions[1] > 0) {
		ySlicePlane->SetSliceIndex(value);
		ui->ySliceLabel->setText(QStringLiteral("Y切片位置: %1").arg(value));
		ui->ySliceWidget->GetRenderWindow()->Render();
	}
}