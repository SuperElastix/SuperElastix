#include <iostream>
#include "elxBlueprint.h"
 
int main()
{
  elx::Blueprint::Pointer blueprint = elx::Blueprint::New();
  elx::Blueprint::ComponentIndexType index0 = blueprint->AddComponent();

  std::cout << "Hello World!" << std::endl;
  std::cout << "Loaded blueprint!" << std::endl;
  return 0;
}
