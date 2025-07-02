#include "BMPVolumeViewer.h"
#include <QApplication>
#include <QSurfaceFormat>

#include <QMainWindow>
#include <QVBoxLayout>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QVTKOpenGLNativeWidget.h>
#include <vtkSmartPointer.h>
#include <vtkBMPReader.h>
#include <vtkImageData.h>
#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>
#include <vtkVolumeProperty.h>
#include <vtkFixedPointVolumeRayCastMapper.h>
#include <vtkVolume.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkImageResample.h>
#include <vtkImageShiftScale.h>
#include <QDebug>
#include <QDir>
#include <vtkImageData.h>
#include <vtkPointData.h>

int main(int argc, char *argv[])
{
	//vtkNew<vtkBMPReader> testReader;
	//testReader->SetFileName("C:/Users/22594/Desktop/imgProcess/result/OS_CSIM_without_using_dual_image/normalized/depth_0.00um.bmp");
	//testReader->Update();  // 检查是否报同样错误

	//// 1. 检查错误状态
	//if (testReader->GetErrorCode() != 0) {
	//	qDebug() << "Error code: " << testReader->GetErrorCode();
	//	// 获取错误描述（通过输出窗口）
	//	qDebug() << "Error message: " << vtkErrorCode::GetStringFromErrorCode(testReader->GetErrorCode());
	//	return 0;
	//}

	//// 2. 检查输出图像是否存在
	//vtkImageData* imageData = testReader->GetOutput();
	//if (!imageData) {
	//	qDebug() << "imageData not found";
	//	return 0;
	//}

	//// 3. 检查图像尺寸
	//int* dims = imageData->GetDimensions();
	//if (dims[0] <= 1 || dims[1] <= 1) {
	//	qDebug() << "invalid image dimensions: " << dims[0] << "x" << dims[1];
	//	return 0;
	//}

	//// 4. 检查像素值范围
	//double range[2];
	//imageData->GetScalarRange(range);
	//qDebug() << "ScalarRange: [" << range[0] << ", " << range[1] << "]";

	//// 5. 检查通道数
	//int components = imageData->GetNumberOfScalarComponents();
	//qDebug() << "Number of scalar components: " << components;
	//if (components != 1 && components != 3) {
	//	qDebug() << "unsupported scalar components: " << components;
	//}

	//vtkNew<vtkImageViewer2> imageViewer;
	//imageViewer->SetInputData(imageData);
	//imageViewer->SetSlice(0);  // 显示第一层（对于2D图像）

	//vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
	//imageViewer->SetupInteractor(renderWindowInteractor);
	//imageViewer->GetRenderer()->ResetCamera();

	//qDebug() << "Show image ... press q to quit";
	//imageViewer->Render();
	//renderWindowInteractor->Start();


	QApplication app(argc, argv);

	// 设置OpenGL表面格式
	QSurfaceFormat::setDefaultFormat(QVTKOpenGLNativeWidget::defaultFormat());

	BMPVolumeViewer viewer;
	viewer.show();

	return app.exec();


	/*

	QApplication app(argc, argv);

	// 创建主窗口
	QMainWindow mainWindow;
	mainWindow.setWindowTitle("VTK BMP序列三维体渲染");
	mainWindow.resize(1000, 800);

	// 创建中央部件和布局
	QWidget *centralWidget = new QWidget(&mainWindow);
	QVBoxLayout *layout = new QVBoxLayout(centralWidget);
	mainWindow.setCentralWidget(centralWidget);

	// 初始化VTK部件
	QVTKOpenGLNativeWidget *vtkWidget = new QVTKOpenGLNativeWidget();
	layout->addWidget(vtkWidget, 1);

	// 创建控制面板
	QWidget *controlPanel = new QWidget();
	QHBoxLayout *controlLayout = new QHBoxLayout(controlPanel);

	QLabel *opacityLabel = new QLabel("不透明度:");
	QSlider *opacitySlider = new QSlider(Qt::Horizontal);
	opacitySlider->setRange(0, 100);
	opacitySlider->setValue(50);

	QLabel *colorLabel = new QLabel("颜色映射:");
	QSlider *colorSlider = new QSlider(Qt::Horizontal);
	colorSlider->setRange(0, 100);
	colorSlider->setValue(50);

	QPushButton *resetViewBtn = new QPushButton("重置视图");

	controlLayout->addWidget(opacityLabel);
	controlLayout->addWidget(opacitySlider);
	controlLayout->addWidget(colorLabel);
	controlLayout->addWidget(colorSlider);
	controlLayout->addWidget(resetViewBtn);

	layout->addWidget(controlPanel);

	// 创建BMP序列读取器
	vtkSmartPointer<vtkBMPReader> bmpReader = vtkSmartPointer<vtkBMPReader>::New();
	bmpReader->SetFilePrefix("C:/Users/22594/Desktop/imgProcess/result/OS_CSIM_without_using_dual_image/normalized/depth_"); // 文件前缀
	bmpReader->SetFilePattern("%s%d.00um.bmp");  // 文件名格式 (e.g., depth_0.00um.bmp)
	bmpReader->SetDataExtent(0, 511, 0, 511, 0, 100); // Z范围对应帧数
	bmpReader->SetDataSpacing(1, 1, 1);
	bmpReader->SetDataOrigin(0, 0, 0);
	bmpReader->SetAllow8BitBMP(true);  // 允许8位BMP
	bmpReader->Update();

	// 检查数据是否成功读取
	if (bmpReader->GetOutput()->GetPointData()->GetScalars() == nullptr) {
		qDebug() << "错误：未读取到图像数据！";
		return -1;
	}

	// 为提升性能，降低采样率
	vtkSmartPointer<vtkImageResample> resample = vtkSmartPointer<vtkImageResample>::New();
	resample->SetInputConnection(bmpReader->GetOutputPort());
	resample->SetAxisMagnificationFactor(0, 0.5); // X方向降采样
	resample->SetAxisMagnificationFactor(1, 0.5); // Y方向降采样
	resample->SetAxisMagnificationFactor(2, 1.0); // Z方向保持原样

	// 创建颜色传输函数（伪彩色映射）
	vtkSmartPointer<vtkColorTransferFunction> colorTransferFunction =
		vtkSmartPointer<vtkColorTransferFunction>::New();
	colorTransferFunction->AddRGBPoint(0, 0.0, 0.0, 0.0);    // 黑色
	colorTransferFunction->AddRGBPoint(64, 1.0, 0.0, 0.0);   // 红色
	colorTransferFunction->AddRGBPoint(128, 1.0, 1.0, 0.0);  // 黄色
	colorTransferFunction->AddRGBPoint(192, 0.0, 1.0, 0.0);  // 绿色
	colorTransferFunction->AddRGBPoint(255, 0.0, 0.0, 1.0);  // 蓝色

	// 创建不透明度传输函数
	vtkSmartPointer<vtkPiecewiseFunction> opacityTransferFunction =
		vtkSmartPointer<vtkPiecewiseFunction>::New();
	opacityTransferFunction->AddPoint(0, 0.0);
	opacityTransferFunction->AddPoint(50, 0.1);
	opacityTransferFunction->AddPoint(100, 0.3);
	opacityTransferFunction->AddPoint(150, 0.6);
	opacityTransferFunction->AddPoint(200, 0.9);
	opacityTransferFunction->AddPoint(255, 1.0);

	// 设置体属性
	vtkSmartPointer<vtkVolumeProperty> volumeProperty =
		vtkSmartPointer<vtkVolumeProperty>::New();
	volumeProperty->SetColor(colorTransferFunction);
	volumeProperty->SetScalarOpacity(opacityTransferFunction);
	volumeProperty->SetInterpolationTypeToLinear();
	volumeProperty->ShadeOn();
	volumeProperty->SetAmbient(0.4);
	volumeProperty->SetDiffuse(0.6);
	volumeProperty->SetSpecular(0.2);

	// 创建体映射器
	vtkSmartPointer<vtkFixedPointVolumeRayCastMapper> volumeMapper =
		vtkSmartPointer<vtkFixedPointVolumeRayCastMapper>::New();
	volumeMapper->SetInputConnection(resample->GetOutputPort());

	// 创建体
	vtkSmartPointer<vtkVolume> volume = vtkSmartPointer<vtkVolume>::New();
	volume->SetMapper(volumeMapper);
	volume->SetProperty(volumeProperty);

	// 配置渲染器
	vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
	renderer->AddVolume(volume);
	renderer->SetBackground(0.2, 0.3, 0.4);
	renderer->ResetCamera();

	// 设置渲染窗口 - 使用GetRenderWindow()
	vtkWidget->GetRenderWindow()->AddRenderer(renderer);

	// 设置交互样式（支持旋转、缩放等操作）
	vtkSmartPointer<vtkRenderWindowInteractor> interactor =
		vtkSmartPointer<vtkRenderWindowInteractor>::New();
	interactor->SetRenderWindow(vtkWidget->GetRenderWindow());

	vtkSmartPointer<vtkInteractorStyleTrackballCamera> style =
		vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
	interactor->SetInteractorStyle(style);

	// 连接控制信号
	QObject::connect(opacitySlider, &QSlider::valueChanged, [&](int value) {
		// 调整不透明度
		double maxOpacity = value / 100.0;
		opacityTransferFunction->RemoveAllPoints();
		opacityTransferFunction->AddPoint(0, 0.0);
		opacityTransferFunction->AddPoint(50, 0.1 * maxOpacity);
		opacityTransferFunction->AddPoint(100, 0.3 * maxOpacity);
		opacityTransferFunction->AddPoint(150, 0.6 * maxOpacity);
		opacityTransferFunction->AddPoint(200, 0.9 * maxOpacity);
		opacityTransferFunction->AddPoint(255, 1.0 * maxOpacity);
		vtkWidget->GetRenderWindow()->Render();
	});

	QObject::connect(colorSlider, &QSlider::valueChanged, [&](int value) {
		// 调整颜色映射
		colorTransferFunction->RemoveAllPoints();
		double hue = value / 100.0;

		colorTransferFunction->AddRGBPoint(0, 0.0, 0.0, 0.0);
		colorTransferFunction->AddRGBPoint(64, 1.0, hue, 0.0);
		colorTransferFunction->AddRGBPoint(128, 1.0, 1.0, hue);
		colorTransferFunction->AddRGBPoint(192, hue, 1.0, 0.0);
		colorTransferFunction->AddRGBPoint(255, 0.0, hue, 1.0);

		vtkWidget->GetRenderWindow()->Render();
	});

	QObject::connect(resetViewBtn, &QPushButton::clicked, [&]() {
		renderer->ResetCamera();
		vtkWidget->GetRenderWindow()->Render();
	});

	// 显示主窗口
	mainWindow.show();

	// 开始交互 - 必须在显示窗口后初始化
	interactor->Initialize();
	interactor->Start();

	return app.exec();

	*/
}