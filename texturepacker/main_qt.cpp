#include <filesystem>
#include <iostream>
#include <QtCore/qsize.h>
#include <QtGui>
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qapplication.h>
#include <QtWidgets/QMessageBox>

// 矩形打包测试
// #define TEST_RECT_PACKER
// 图片合成开关
#define TEST_IMAGE_PACKER
#include "image_merge.hpp"

#define RES_DIR "/Users/dt/work/code/mapstyle/texturepacker/res/"
// #define RES_DIR "/Users/dt/Downloads/mapcfg_straight@2x/"
class Visualizer : public QWidget
{
public:
    Visualizer(QWidget* parent = 0)
        : QWidget(parent)
    {
        resize(1024, 1024);
#ifdef TEST_IMAGE_PACKER
        img_packer_test.loadImages(RES_DIR);
#endif
    }

protected:
    // bool eventFilter(QObject* pObj, QEvent* pEvent) {
    // // We need to check for both types of mouse release, because it can vary on which type happens when resizing.
    //     if ((pEvent->type() == QEvent::MouseButtonRelease) || (pEvent->type() == QEvent::NonClientAreaMouseButtonRelease)) {
    //         QMouseEvent* pMouseEvent = dynamic_cast<QMouseEvent*>(pEvent);
    //         // if ((pMouseEvent->button() == Qt::MouseButton::LeftButton)) {
    //         //     // printf("Gotcha!\");
    //         //     // m_bUserIsResizing = false; // reset user resizing flag
    //         // }
    //     }
    //     return QObject::eventFilter(pObj, pEvent); // pass it on without eating it
    // }
    void closeEvent(QCloseEvent * event){
        QMessageBox::StandardButton result =  
                    QMessageBox::information(this, tr("标题"), tr("是否保存当前合批结果"),
                                    QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes);
        switch (result)
        {
        case QMessageBox::Yes:
#ifdef TEST_IMAGE_PACKER
            merge_image_dir = RES_DIR;
            merge_image_dir += "../";
            update_merge_image = img_packer_test.packer.merge(merge_image_dir.c_str(), merge_image_name.c_str());
#endif
            event->ignore();
            break;
        
        default:
            break;
        }
    }
    void resizeEvent(QResizeEvent *event){
        QSize view_sz = event->size();
#ifdef TEST_RECT_PACKER
        rect_packer_test.resize(view_sz.width(), view_sz.height());
#endif

#ifdef TEST_IMAGE_PACKER
        img_packer_test.resize(view_sz.width(), view_sz.height());
#endif
    }
    void paintEvent(QPaintEvent*) {
        QPainter painter(this);
#ifdef TEST_RECT_PACKER
        const QStringList colors = QColor::colorNames();
        const QStringList::const_iterator colorEnd = colors.end();
        QStringList::const_iterator color = colors.begin();
        std::vector<Rect>::const_iterator rect = rect_packer_test.rects.begin();
        const std::vector<Rect>::const_iterator end = rect_packer_test.rects.end();
        while (rect != end) {
            if (color == colorEnd)
                color = colors.begin();
            // qDebug() << "filling" << rect->x << rect->y << rect->width << rect->height;
            painter.fillRect(rect->x, rect->y, rect->width, rect->height, QColor(*color));
            ++color;
            ++rect;
        }
#endif

#ifdef TEST_IMAGE_PACKER
        if(img_packer_test.packer.imageList.size() != img_packer_test.packer.rcList.size())
            return ;
        
        if(update_merge_image){
            painter.drawPixmap({0, 0, img_packer_test.packer.packer_w, img_packer_test.packer.packer_h},
                        QPixmap((merge_image_dir + merge_image_name).c_str()));
            update_merge_image = false;
        }
        else {
            int idnum = 0;
            for(const auto & img : img_packer_test.packer.imageList){
                std::string img_dir = RES_DIR;
                img_dir += img.name;
                auto pos = img_packer_test.packer.rcList[idnum++];
                QRect rc(pos.x, pos.y, pos.width, pos.height);
                painter.drawPixmap(rc,QPixmap(img_dir.c_str()));
            }
        }
#endif
    }

private:
#ifdef TEST_RECT_PACKER
    RectPackerTest rect_packer_test;
#endif
#ifdef TEST_IMAGE_PACKER    
    bool update_merge_image = false;
    std::string merge_image_dir = RES_DIR;
    std::string merge_image_name = "test";
    ImagePackerTest img_packer_test;
#endif
};

int main(int argc, char ** argv)
{
    QApplication app(argc, argv);

    Visualizer vis;
    vis.show();

    return app.exec();
}
