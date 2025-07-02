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

private:
	void setupVTKPipeline();
	void updateColorTransferFunction(double hue);
	void updateOpacityTransferFunction(double maxOpacity);

	Ui::BMPVolumeViewer *ui;

	// VTK 管线对象
	vtkSmartPointer<vtkBMPReader> bmpReader;
	vtkSmartPointer<vtkImageResample> resample;
	vtkSmartPointer<vtkColorTransferFunction> colorTransferFunction;
	vtkSmartPointer<vtkPiecewiseFunction> opacityTransferFunction;
	vtkSmartPointer<vtkVolumeProperty> volumeProperty;
	vtkSmartPointer<vtkFixedPointVolumeRayCastMapper> volumeMapper;
	vtkSmartPointer<vtkVolume> volume;
	vtkSmartPointer<vtkRenderer> renderer;
};
#endif // BMPVOLUMEVIEWER_H