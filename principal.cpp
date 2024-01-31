#include "principal.h"
#include "ui_principal.h"

#define DEFAULT_ANCHO 3

Principal::Principal(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Principal)
{
    ui->setupUi(this);
    // Instanciando la imagen (creando)
    mImagen = new QImage(this->size(),QImage::Format_ARGB32_Premultiplied);
    // Rellenar la imagen de color blanco
    mImagen->fill(Qt::white);
    // Instanciar el Painter a partir de la imagen
    mPainter = new QPainter(mImagen);
    mPainter->setRenderHint(QPainter::Antialiasing);
    // Inicializar otras variables
    mPuedeDibujar = false;
    mColor = Qt::black;
    mAncho = DEFAULT_ANCHO;
    mNumLineas = 0;
    mNumClic = 0;
    // La forma por defecto es libre
    mForma = Forma::LIBRE;
    elegirForma();
}

Principal::~Principal()
{
    delete ui;
    delete mPainter;
    delete mImagen;
}

void Principal::paintEvent(QPaintEvent *event)
{
    // Crear el painter local de la ventana principal
    QPainter painter(this);
    // Dibujar la imagen
    painter.drawImage(0, 0, *mImagen);
    // Acepatr el evento
    event->accept();
}

void Principal::mousePressEvent(QMouseEvent *event)
{

    switch (mForma) {
    case Forma::LIBRE:
        // Levanta la bandera (para que se pueda dibujar libre)
        mPuedeDibujar = true;
        // Captura la posición (punto x,y) del mouse
        mInicial = event->pos();
        break;
    case Forma::LINEAS:
        if (mNumClic == 0){
            mInicial = event->pos();
        }
        mNumClic++;
        break;
    case Forma::RECTANGULO:
        if (mNumClic == 0){
            mInicial = event->pos();
        }
        mNumClic++;
        break;
    case Forma::CIRCUNFERENCIA:
        if (mNumClic == 0){
            mInicial = event->pos();
        }
        mNumClic++;
        break;
    }



    // Acepta el evento
    event->accept();
}

void Principal::mouseMoveEvent(QMouseEvent *event)
{
    // Validar si se puede dibujar
    if ( !mPuedeDibujar ) {
        // Acepta el evento
        event->accept();
        // Salir del método
        return;
    }
    if (mForma == Forma::LIBRE){
        // Capturar el punto a donde se mueve el mouse
        mFinal = event->pos();
        // Crear un pincel y establecer atributos
        QPen pincel;
        pincel.setColor(mColor);
        pincel.setWidth(mAncho);
        // Dibujar una linea
        mPainter->setPen(pincel);
        mPainter->drawLine(mInicial, mFinal);
        // Mostrar el número de líneas en la barra de estado
        ui->statusbar->showMessage("Número de líneas: " + QString::number(++mNumLineas));
        // Actualizar la interfaz (repinta con paintEvent)
        update();
        // actualizar el punto inicial
        mInicial = mFinal;
    }
}

void Principal::mouseReleaseEvent(QMouseEvent *event)
{
    if(mForma == Forma::LINEAS && mNumClic == 2){
        mFinal = event->pos();
        // Crear un pincel y establecer atributos
        QPen pincel;
        pincel.setColor(mColor);
        pincel.setWidth(mAncho);
        // Dibujar una linea
        mPainter->setPen(pincel);
        mPainter->drawLine(mInicial, mFinal);
        mNumClic = 0;
        // Actualizar la interfaz (repinta con paintEvent)
        update();
    }else if (mForma == Forma::RECTANGULO && mNumClic == 2) {
        // Finaliza la creación de un rectángulo
        mFinal = event->pos();
        // Crear un pincel y establecer atributos
        QPen pincel;
        pincel.setColor(mColor);
        pincel.setWidth(mAncho);
        // Dibujar el rectángulo
        mPainter->setPen(pincel);
        mPainter->drawRect(QRect(mInicial, mFinal));
        mNumClic = 0;
        // Actualizar la interfaz (repinta con paintEvent)
        update();
        // Bajar la bandera (no se puede dibujar)
    }else if (mForma == Forma::CIRCUNFERENCIA && mNumClic == 2) {
        // Finaliza la creación de una circunferencia
        mFinal = event->pos();
        // Calcular el radio como la distancia entre el centro y el punto final
        int radio = QLineF(mInicial, mFinal).length();
        // Crear un pincel y establecer atributos
        QPen pincel;
        pincel.setColor(mColor);
        pincel.setWidth(mAncho);
        // Dibujar la circunferencia
        mPainter->setPen(pincel);
        mPainter->drawEllipse(mInicial, radio, radio);
        mNumClic = 0;
        // Actualizar la interfaz (repinta con paintEvent)
        update();
        }  else {
        // Bajar la bandera (no se puede dibujar)
        mPuedeDibujar = false;
    }

    // Aceptar el evento
    event->accept();

}


void Principal::on_actionAncho_triggered()
{
    mAncho = QInputDialog::getInt(this,
                                  "Ancho del pincel",
                                  "Ingrese el ancho del pincel de dibujo",
                                  mAncho,
                                  1, 20);
}

void Principal::on_actionSalir_triggered()
{
    this->close();
}

void Principal::on_actionColor_triggered()
{
    mColor = QColorDialog::getColor(mColor,
                                    this,
                                    "Color del pincel");
}

void Principal::on_actionNuevo_triggered()
{
    mImagen->fill(Qt::white);
    mNumLineas = 0;
    update();
}

void Principal::on_actionGuardar_triggered()
{
    // Abrir cuadro de diálogo para obtener el nombre del archivo
    QString nombreArchivo = QFileDialog::getSaveFileName(this,
                                                         "Guardar imagen",
                                                         QString(),
                                                         "Imágenes .png (*.png)");
    // Validar que el nombre del archivo no sea vacío
    if ( !nombreArchivo.isEmpty() ){
        // Guardar imagen
        if (mImagen->save(nombreArchivo)){
            // Si todo va bien, muestra un mensaje de información
            QMessageBox::information(this,
                                     "Guardar imagen",
                                     "Archivo almacenado en: " + nombreArchivo);
        } else{
            // Si hay algún error, muestro advertencia
            QMessageBox::warning(this,
                                 "Guardar imagen",
                                 "No se pudo almacenar la imagen.");
        }
    }
}

void Principal::elegirForma()
{
    switch(mForma){
    case Forma::LINEAS:
        ui->actionLineas->setChecked(true);
        ui->actionLibre->setChecked(false);
        ui->actionRect_nculos->setChecked(false);
        ui->actionCircunferencias->setChecked(false);
        break;
    case Forma::LIBRE:
        ui->actionLineas->setChecked(false);
        ui->actionLibre->setChecked(true);
        ui->actionRect_nculos->setChecked(false);
        ui->actionCircunferencias->setChecked(false);
        break;
    case Forma::RECTANGULO:
        ui->actionLineas->setChecked(false);
        ui->actionLibre->setChecked(false);
        ui->actionRect_nculos->setChecked(true);
        ui->actionCircunferencias->setChecked(false);
        break;
    case Forma::CIRCUNFERENCIA:
        ui->actionLineas->setChecked(false);
        ui->actionLibre->setChecked(false);
        ui->actionRect_nculos->setChecked(false);
        ui->actionCircunferencias->setChecked(true);
        break;
    }
}


void Principal::on_actionLibre_triggered()
{
    mForma = Forma::LIBRE;
    elegirForma();
}


void Principal::on_actionLineas_triggered()
{
    mForma = Forma::LINEAS;
    elegirForma();
}


void Principal::on_actionRect_nculos_triggered()
{
    mForma = Forma::RECTANGULO;
    elegirForma();
}


void Principal::on_actionCircunferencias_triggered()
{
    mForma = Forma::CIRCUNFERENCIA;
    elegirForma();
}

