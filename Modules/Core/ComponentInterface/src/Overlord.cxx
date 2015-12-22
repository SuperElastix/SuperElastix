#include "Overlord.h"

namespace selx
{
  void Overlord::SetBlueprint(const Blueprint::Pointer blueprint)
  {
    m_Blueprint = blueprint;
    return;

  }
  bool Overlord::UpdateSelectors()
  {
    bool allUniqueComponents = true;
    Blueprint::ComponentIteratorPairType componentItPair = this->m_Blueprint->GetComponentIterator();
    Blueprint::ComponentIteratorPairType::first_type componentIt;
    Blueprint::ComponentIteratorPairType::second_type componentItEnd = componentItPair.second;
    for (componentIt = componentItPair.first; componentIt != componentItEnd; ++componentIt)
    {
      ComponentSelector::NumberOfComponentsType numberOfComponents = this->m_ComponentSelectorContainer[*componentIt]->UpdatePossibleComponents();

      // The current idea of the configuration setup is that the number of 
      // possible components at a node can only be reduced by adding criteria.
      // If a node has 0 possible components, the configuration is aborted (with an exception)
      // If all nodes have exactly 1 possible component, no more criteria are needed.
      if (numberOfComponents == 0)
      {
        itkExceptionMacro("Too many criteria for component");
      }
      else if (numberOfComponents > 1)
      {
        allUniqueComponents = false;
      }
      std::cout << "blueprint node " << *componentIt << " has " << numberOfComponents << " components" << std::endl;

    }
    return allUniqueComponents;
  }

  bool Overlord::Configure()
  {
    bool allUniqueComponents;
    this->ApplyNodeConfiguration();
    allUniqueComponents = this->UpdateSelectors();
    std::cout << "Based on Component Criteria unique components could " << (allUniqueComponents ? "":"not ") << "be selected" << std::endl;

    this->ApplyConnectionConfiguration();
    allUniqueComponents = this->UpdateSelectors();
    std::cout << "By adding Connection Criteria unique components could " << (allUniqueComponents ? "" : "not ") << "be selected" << std::endl;
    return allUniqueComponents;
  }
  void Overlord::ApplyNodeConfiguration()
  {
    Blueprint::ComponentIteratorPairType componentItPair = this->m_Blueprint->GetComponentIterator();
    Blueprint::ComponentIteratorPairType::first_type componentIt;
    Blueprint::ComponentIteratorPairType::second_type componentItEnd = componentItPair.second;
    for (componentIt = componentItPair.first; componentIt != componentItEnd; ++componentIt)
    {
      ComponentSelectorPointer currentComponentSelector = ComponentSelector::New();
      Blueprint::ParameterMapType currentProperty = this->m_Blueprint->GetComponent(*componentIt);
      currentComponentSelector->SetCriteria(currentProperty);
      this->m_ComponentSelectorContainer.push_back(currentComponentSelector);
    }
    return;
  }
  void Overlord::ApplyConnectionConfiguration()
  {
    //typedef Blueprint::OutputIteratorType OutputIteratorType;
    //typedef Blueprint::OutputIteratorPairType OutputIteratorPairType;

    //TODO: these loops have to be redesigned for a number of reasons:
    // - They rely on the assumption that the index of the vector equals the componentIndex in blueprint
    // - Tedious, integer indexing.
    //
    // We might consider copying the blueprint graph to a component selector 
    // graph, such that all graph operations correspond
    
    // Or loop over connections:
    //Blueprint::ConnectionIteratorPairType connectionItPair = this->m_Blueprint->GetConnectionIterator();
    //Blueprint::ConnectionIteratorPairType::first_type  connectionIt;
    //Blueprint::ConnectionIteratorPairType::second_type  connectionItEnd = connectionItPair.second;
    //int count = 0;
    //for (connectionIt = connectionItPair.first; connectionIt != connectionItEnd; ++connectionIt)
    //{
    //}

    
    Blueprint::ComponentIndexType index;
    for (index = 0; index < this->m_ComponentSelectorContainer.size(); ++index)
    {
      Blueprint::OutputIteratorPairType ouputItPair = this->m_Blueprint->GetOutputIterator(index);
      Blueprint::OutputIteratorPairType::first_type ouputIt;
      Blueprint::OutputIteratorPairType::second_type ouputItEnd = ouputItPair.second;
      for (ouputIt = ouputItPair.first; ouputIt != ouputItEnd; ++ouputIt)
      {
        //TODO check direction upstream/downstream input/output source/target
        Blueprint::ParameterMapType currentProperty = this->m_Blueprint->GetConnection(ouputIt->m_source, ouputIt->m_target);
        for (Blueprint::ParameterMapType::const_iterator it = currentProperty.begin(); it != currentProperty.cend(); ++it)
        {
          if (it->first == "NameOfInterface")
          {
            ComponentBase::CriteriaType additionalSourceCriteria;            
            additionalSourceCriteria.insert(ComponentBase::CriteriumType("HasProvidingInterface", it->second));

            ComponentBase::CriteriaType additionalTargetCriteria;
            additionalTargetCriteria.insert(ComponentBase::CriteriumType("HasAcceptingInterface", it->second));

            this->m_ComponentSelectorContainer[ouputIt->m_source]->AddCriteria(additionalSourceCriteria);
            this->m_ComponentSelectorContainer[ouputIt->m_target]->AddCriteria(additionalTargetCriteria);
          }
        }
      }
    }

    return;
  }
} // end namespace selx

