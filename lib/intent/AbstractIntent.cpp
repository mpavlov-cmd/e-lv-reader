#include <AbstractIntent.h>

const ActionResult ActionResult::VOID = {ActionRetultType::VOID, 0, IntentArgument::NO_ARG};

AbstractIntent::AbstractIntent(QueueHandle_t& mEventQueue): eventQueue(mEventQueue)
{
}
