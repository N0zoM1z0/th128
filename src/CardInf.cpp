#include "CardInf.hpp"

void CardInf::EnableChains()
{
    CardInf *card = g_CardInf;
    if (card->calcElement != 0)
        card->calcElement->flags |= 2U;
    if (card->drawElement != 0)
        card->drawElement->flags |= 2U;
}
