#include "Overlord.h"

namespace selx
{
  Overlord::Overlord()
  {
    this->m_SinkComponents = SinkComponentsContainerType::New();
    this->m_SourceComponents = SourceComponentsContainerType::New();
  }
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
      std::cout << "blueprint node " << *componentIt << " has selected" << numberOfComponents << " components" << std::endl;

    }
    return allUniqueComponents;
  }

  bool Overlord::Configure()
  {
    bool isSuccess(false);
    bool allUniqueComponents;
    this->ApplyNodeConfiguration();
    allUniqueComponents = this->UpdateSelectors();
    std::cout << "Based on Component Criteria unique components could " << (allUniqueComponents ? "":"not ") << "be selected" << std::endl;

    this->ApplyConnectionConfiguration();
    allUniqueComponents = this->UpdateSelectors();
    std::cout << "By adding Connection Criteria unique components could " << (allUniqueComponents ? "" : "not ") << "be selected" << std::endl;

    this->FindSources();
    std::cout << "Found " << this->m_SourceComponents->size() << " Source Components" << std::endl;
    this->FindSinks();
    std::cout << "Found " << this->m_SinkComponents->size() << " Sink Components" << std::endl;
    this->ConnectSources();
    this->ConnectSinks();

    if (allUniqueComponents)
    {
      isSuccess = this->ConnectComponents();
    }

    return isSuccess;
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
            additionalSourceCriteria.insert(ComponentBase::CriterionType("HasProvidingInterface", it->second));

            ComponentBase::CriteriaType additionalTargetCriteria;
            additionalTargetCriteria.insert(ComponentBase::CriterionType("HasAcceptingInterface", it->second));

            this->m_ComponentSelectorContainer[ouputIt->m_source]->AddCriteria(additionalSourceCriteria);
            this->m_ComponentSelectorContainer[ouputIt->m_target]->AddCriteria(additionalTargetCriteria);
          }
        }
      }
    }

    return;
  }
  bool Overlord::ConnectComponents()
  {

    //TODO: redesign loops, see ApplyConnectionConfiguration()
    Blueprint::ComponentIndexType index;
    for (index = 0; index < this->m_ComponentSelectorContainer.size(); ++index)
    {
      Blueprint::OutputIteratorPairType ouputItPair = this->m_Blueprint->GetOutputIterator(index);
      Blueprint::OutputIteratorPairType::first_type ouputIt;
      Blueprint::OutputIteratorPairType::second_type ouputItEnd = ouputItPair.second;
      for (ouputIt = ouputItPair.first; ouputIt != ouputItEnd; ++ouputIt)
      {
        //TODO check direction upstream/downstream input/output source/target
        //TODO GetComponent returns NULL if possible components !=1 we can check for that, but Overlord::UpdateSelectors() does something similar.
        ComponentBase::Pointer sourceComponent = this->m_ComponentSelectorContainer[ouputIt->m_source]->GetComponent();
        ComponentBase::Pointer targetComponent = this->m_ComponentSelectorContainer[ouputIt->m_target]->GetComponent();
        targetComponent->ConnectFrom(sourceComponent);
      }
    }
    //TODO should we communicate by exceptions instead of returning booleans?
    return true;
  }
  bool Overlord::FindSources()
  {
    
    const CriterionType sourceCriterion = CriterionType("HasProvidingInterface", ParameterValueType(1, "SourceInterface"));
   
    // TODO redesign ComponentBase class to accept a single criterion instead of a criteria mapping.
    CriteriaType sourceCriteria;
    sourceCriteria.insert(sourceCriterion);
   

    for (auto && componentSelector : (this->m_ComponentSelectorContainer))
    {
      ComponentBase::Pointer component = componentSelector->GetComponent();
      if (component->MeetsCriteria(sourceCriteria)) // TODO MeetsCriterion
      {
        //SourceInterface* provingSourceInterface = dynamic_cast<SourceInterface*> (&(*component));
        //if (provingSourceInterface == nullptr) // is actually a double-check for sanity: based on criterion cast should be successful
        //{
        //  itkExceptionMacro("dynamic_cast<SourceInterface*> fails, but based on component criterion it shouldn't")
        //}
        //this->m_SourceComponents->push_back(provingSourceInterface);

        this->m_SourceComponents->push_back(component);
      }
    }

    return true;
  }

  bool Overlord::FindSinks()
  {
    const CriterionType sinkCriterion = CriterionType("HasProvidingInterface", ParameterValueType(1, "SinkInterface"));

    // TODO redesign ComponentBase class to accept a single criterion instead of a criteria mapping.
    CriteriaType sinkCriteria;
    sinkCriteria.insert(sinkCriterion);

    for (auto && componentSelector : (this->m_ComponentSelectorContainer))
    {
      ComponentBase::Pointer component = componentSelector->GetComponent();
      if (component->MeetsCriteria(sinkCriteria))  // TODO MeetsCriterion
      {
        //SinkInterface* provingSinkInterface = dynamic_cast<SinkInterface*> (&(*component));
        //if (provingSinkInterface == nullptr) // is actually a double-check for sanity: based on criterion cast should be successful
        //{
        //  itkExceptionMacro("dynamic_cast<SinkInterface*> fails, but based on component criterion it shouldn't")
        //}
        //this->m_SinkComponents->push_back(provingSinkInterface);
        this->m_SinkComponents->push_back(component);
      }
    }

    return true;
  }


  bool Overlord::ConnectSources()
  {
    // For testing purposes, all Sources are connected to an ImageReader
    itk::ImageFileReader<itk::Image<double, 3>>::Pointer reader = itk::ImageFileReader<itk::Image<double, 3>>::New();
    
    reader->SetFileName("e:/data/smalltest/small3d.mhd");

    for (auto && sourceComponent : *(this->m_SourceComponents)) // auto&& preferred?
    {
      SourceInterface* provingSourceInterface = dynamic_cast<SourceInterface*> (&(*sourceComponent));
      if (provingSourceInterface == nullptr) // is actually a double-check for sanity: based on criterion cast should be successful
      {
        itkExceptionMacro("dynamic_cast<SourceInterface*> fails, but based on component criterion it shouldn't")
      }
      provingSourceInterface->ConnectToOverlordSource(reader->GetOutput());
    }

    return true;
  }
  bool Overlord::ConnectSinks()
  {
    // For testing purposes, all Sources are connected to an ImageReader
    itk::ImageFileWriter<itk::Image<double, 3>>::Pointer writer = itk::ImageFileWriter<itk::Image<double, 3>>::New();
    writer->SetFileName("testoutput.mhd");

    for (auto const & sinkComponent : *(this->m_SinkComponents)) // auto&& preferred?
    {
      SinkInterface* provingSinkInterface = dynamic_cast<SinkInterface*> (&(*sinkComponent));
      if (provingSinkInterface == nullptr) // is actually a double-check for sanity: based on criterion cast should be successful
      {
        itkExceptionMacro("dynamic_cast<SinkInterface*> fails, but based on component criterion it shouldn't")
      }
      provingSinkInterface->ConnectToOverlordSink((itk::SmartPointer<itk::Object>) writer);
    }
    return true;

    return true;
  }

} // end namespace selx

