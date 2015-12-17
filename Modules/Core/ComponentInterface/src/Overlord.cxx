#include "Overlord.h"

namespace selx
{
  void Overlord::SetBlueprint(const Blueprint::Pointer blueprint)
  {
    m_Blueprint = blueprint;
    return;

  }
  int Overlord::Update()
  {
    Blueprint::ComponentIteratorPairType componentItPair = this->m_Blueprint->GetComponentIterator();
    Blueprint::ComponentIteratorPairType::first_type componentIt;
    Blueprint::ComponentIteratorPairType::second_type componentItEnd = componentItPair.second;
    int count = 0;
    for (componentIt = componentItPair.first; componentIt != componentItEnd; ++componentIt)
    {
      ++count;
    }
    return count;
  }

} // end namespace selx

