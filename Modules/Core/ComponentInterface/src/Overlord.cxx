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
      ComponentSelectorPointer currentComponentSelector = ComponentSelector::New();
      Blueprint::ParameterMapType currentProperty = this->m_Blueprint->GetComponent(*componentIt);
      currentComponentSelector->SetCriteria(currentProperty);
      this->m_ComponentSelectorContainer.push_back(currentComponentSelector);
      ++count;
    }
    return count;
  }

} // end namespace selx

