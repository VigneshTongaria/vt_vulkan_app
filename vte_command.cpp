#include "vte_command.hpp"


VteCommand::VteCommand(VteDevice& device) : device(device)
{
    createCommandPool();
}

void VteCommand::createCommandPool()
{

}
