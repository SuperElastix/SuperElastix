#include "Overlord.h"

namespace selx
{
  void Overlord::SetBlueprint(const Blueprint::Pointer blueprint)
  {
    m_Blueprint = blueprint;
    return;

  }
  void Overlord::Configure()
  {
    this->ApplyNodeConfiguration();
    this->ApplyConnectionConfiguration();
  }
  bool Overlord::ApplyNodeConfiguration()

  {
    Blueprint::ComponentIteratorPairType componentItPair = this->m_Blueprint->GetComponentIterator();
    Blueprint::ComponentIteratorPairType::first_type componentIt;
    Blueprint::ComponentIteratorPairType::second_type componentItEnd = componentItPair.second;
    for (componentIt = componentItPair.first; componentIt != componentItEnd; ++componentIt)
    {
      ComponentSelectorPointer currentComponentSelector = ComponentSelector::New();
      Blueprint::ParameterMapType currentProperty = this->m_Blueprint->GetComponent(*componentIt);
      currentComponentSelector->SetCriteria(currentProperty);
      ComponentSelector::NumberOfComponentsType numberOfComponents = currentComponentSelector->UpdatePossibleComponents();
      //if (numberOfComponents == 0)
      //{
      //  itkExceptionMacro("Too many criteria for component");
      //}
      std::cout << "blueprint node " << *componentIt << " has " << numberOfComponents << " components" << std::endl;
      this->m_ComponentSelectorContainer.push_back(currentComponentSelector);
    }
    return true; // TODO return false if any componentselector returns 0 possible components
  }
  bool Overlord::ApplyConnectionConfiguration()
  {
    //typedef Blueprint::OutputIteratorType OutputIteratorType;
    //typedef Blueprint::OutputIteratorPairType OutputIteratorPairType;

    //TODO: these loops have to be redesigned for a number of reasons:
    // - They rely on the assumption that the index of the vector equals the componentIndex in blueprint
    // - Tedious, integer indexing.

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
    //ComponentSelectorIteratorType nodeiterator = this->m_ComponentSelectorContainer->begin();


    //OutputIteratorPairType GetOutputIterator(const ComponentIndexType index) {
    //  return boost::out_edges(index, this->m_Graph);
    //}

    Blueprint::ConnectionIteratorPairType connectionItPair = this->m_Blueprint->GetConnectionIterator();
    Blueprint::ConnectionIteratorPairType::first_type  connectionIt;
    Blueprint::ConnectionIteratorPairType::second_type  connectionItEnd = connectionItPair.second;
    //int count = 0;
    for (connectionIt = connectionItPair.first; connectionIt != connectionItEnd; ++connectionIt)
    {

    }
    return true; // TODO return false if any componentselector returns 0 possible components
  }
} // end namespace selx

