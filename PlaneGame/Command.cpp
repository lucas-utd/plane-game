#include "Command.h"

Command::Command()
: action{}
, category{ static_cast<unsigned int>(Category::Type::None) }
{
}
