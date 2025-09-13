#include "other.h"

other::other(): QObject(), QGraphicsItem()
{

}
int other::type() const
{
    return Type;
}
