#include "preview.hpp"

#include "src/ui/colors.hpp"
#include "src/models/filetrace.hpp"

#include <QGraphicsScene>
#include <QRect>
#include <QLabel>

using namespace view;

Preview::Preview(std::shared_ptr<Trace> trace, QWidget *parent) : QGraphicsView(parent),
                                                                  scene(new QGraphicsScene(this)), trace(std::move(trace)) {
    setScene(scene);
    scene->addText("Placeholder");
//    const auto slots = trace->getSlots();
//
//    size_t i = 0;
//    for (auto slot : slots) {
//        auto rec = new QRectF();
//        //scene->addRect(rec);
//        ++i;
//    }
}