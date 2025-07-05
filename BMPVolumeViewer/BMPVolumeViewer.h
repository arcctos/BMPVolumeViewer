#ifndef BMPVOLUMEVIEWER_H
#define BMPVOLUMEVIEWER_H

#include <QMainWindow>
#include <QVTKOpenGLNativeWidget.h>
#include <vtkSmartPointer.h>
#include <vtkBMPReader.h>
#include <vtkImageResample.h>
#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>
#include <vtkVolumeProperty.h>
#include <vtkFixedPointVolumeRayCastMapper.h>
#include <vtkVolume.h>
#include <vtkRenderer.h>
#include <vtkImageData.h>
#include <vtkPointData.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkImagePlaneWidget.h>
#include <vtkOrientationMarkerWidget.h> // 添加坐标轴控件
#include <vtkAxesActor.h> // 添加坐标轴
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui { class BMPVolumeViewer; }
QT_END_NAMESPACE

class BMPVolumeViewer : public QMainWindow
{
	Q_OBJECT

public:
	BMPVolumeViewer(QWidget *parent = nullptr);
	~BMPVolumeViewer();

private slots:
	void on_browseButton_clicked();
	void on_loadButton_clicked();
	void on_opacitySlider_valueChanged(int value);
	void on_colorSlider_valueChanged(int value);
	void on_resetViewButton_clicked();
	void on_xSliceSlider_valueChanged(int value);
	void on_ySliceSlider_valueChanged(int value);

private:
	void setupVTKPipeline();
	void updateColorTransferFunction(double hue);
	void updateOpacityTransferFunction(double maxOpacity);
	void setupSliceViews();
	void setupAxesWidget(); // 添加坐标轴设置函数

	vtkSmartPointer<vtkLookupTable> createLookupTableFromColorTF();
	void updateSliceLookupTables();

	Ui::BMPVolumeViewer *ui;

	// VTK 管线对象
	vtkSmartPointer<vtkBMPReader> bmpReader;
	vtkSmartPointer<vtkImageResample> resample;
	vtkSmartPointer<vtkColorTransferFunction> colorTransferFunction;
	vtkSmartPointer<vtkPiecewiseFunction> opacityTransferFunction;
	vtkSmartPointer<vtkVolumeProperty> volumeProperty;
	vtkSmartPointer<vtkFixedPointVolumeRayCastMapper> volumeMapper;
	vtkSmartPointer<vtkVolume> volume;
	vtkSmartPointer<vtkRenderer> volumeRenderer;
	vtkSmartPointer<vtkOrientationMarkerWidget> axesWidget; // 坐标轴控件

	// 切片视图渲染器
	vtkSmartPointer<vtkRenderer> xSliceRenderer;
	vtkSmartPointer<vtkRenderer> ySliceRenderer;

	// 切片平面控件
	vtkSmartPointer<vtkImagePlaneWidget> xSlicePlane;
	vtkSmartPointer<vtkImagePlaneWidget> ySlicePlane;

	// 数据信息
	int dataDimensions[3];
	double dataSpacing[3];
};
#endif // BMPVOLUMEVIEWER_H