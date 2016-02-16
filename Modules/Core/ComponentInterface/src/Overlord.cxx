#include "Overlord.h"
#include <windows.h>

namespace selx
{
  Overlord::Overlord()
  {
    //this->m_SinkComponents = SinkComponentsContainerType::New();
    //this->m_SourceComponents = SourceComponentsContainerType::New();
    this->m_RunRegistrationComponents = ComponentsContainerType::New();

    // temporary solution
    this->m_Readers2float = Reader2floatContainerType::New();
    this->m_Writers2float = Writer2floatContainerType::New();
    this->m_Readers3double = Reader3doubleContainerType::New();
    this->m_Writers3double = Writer3doubleContainerType::New();
  }

  void Overlord::SetBlueprint(const Blueprint::Pointer blueprint)
  {
    m_Blueprint = blueprint;
    return;

  }

  bool Overlord::Configure()
  {
    bool isSuccess(false);
    bool allUniqueComponents;
    this->ApplyNodeConfiguration();
    allUniqueComponents = this->UpdateSelectors();
    std::cout << "Based on Component Criteria unique components could " << (allUniqueComponents ? "" : "not ") << "be selected" << std::endl;

    this->ApplyConnectionConfiguration();
    allUniqueComponents = this->UpdateSelectors();
    std::cout << "By adding Connection Criteria unique components could " << (allUniqueComponents ? "" : "not ") << "be selected" << std::endl;

    
    this->ConnectSources();

    int numberSources2float = this->m_Readers2float->size(); // temporary solution
    std::cout << "Found " << numberSources2float << " Source Components (2d float)" << std::endl;
    int numberSources3double = this->m_Readers3double->size(); // temporary solution
    std::cout << "Found " << numberSources3double << " Source Components (3d double)" << std::endl;

    this->ConnectSinks();
    int numberSinks2float = this->m_Writers2float->size(); // temporary solution
    std::cout << "Found " << numberSinks2float << " Sink Components (2d float)" << std::endl;
    int numberSinks3double = this->m_Writers3double->size(); // temporary solution
    std::cout << "Found " << numberSinks3double << " Sink Components (3d double)" << std::endl;



    if (allUniqueComponents)
    {
      isSuccess = this->ConnectComponents();
    }

    this->FindRunRegistration();
    return isSuccess;
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
      //
      // Design consideration: should the exception be thrown by this->m_ComponentSelectorContainer[*componentIt]?
      // The (failing) criteria can be printed as well.
      if (numberOfComponents > 1)
      {
        allUniqueComponents = false;
      }
      std::cout << "blueprint node " << *componentIt << " has selected " << numberOfComponents << " components" << std::endl;

    }
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
    //
    // This could be in line with the idea of maintaining 2 graphs: 1 descriptive (= const blueprint) and
    // 1 internal holding to realized components.
    // Manipulating the internal graph (combining component nodes into a hybrid node, duplicating sub graphs, etc)
    // is possible then.
    //
    // Additional redesign consideration: the final graph should hold the realized components at each node and not the 
    // ComponentSelectors that, in turn, hold 1 (or more) component.
    //
    //
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
        targetComponent->AcceptConnectionFrom(sourceComponent);
      }
    }
    //TODO should we communicate by exceptions instead of returning booleans?
    return true;
  }
  bool Overlord::ConnectSources()
  {
    /** Scans all Components to find those with Sourcing capability and store them in SourceComponents list */

    const CriterionType sourceCriterion = CriterionType("HasProvidingInterface", { "SourceInterface" });
   
    int readercounter = 0; // temporary solution for reader filenames

    // TODO redesign ComponentBase class to accept a single criterion instead of a criteria mapping.
    CriteriaType sourceCriteria;
    sourceCriteria.insert(sourceCriterion);

    for (auto && componentSelector : (this->m_ComponentSelectorContainer))
    {
      ComponentBase::Pointer component = componentSelector->GetComponent();
      if (component->MeetsCriteria(sourceCriteria)) // TODO MeetsCriterion
      {
        SourceInterface* provingSourceInterface = dynamic_cast<SourceInterface*> (&(*component));
        if (provingSourceInterface == nullptr) // is actually a double-check for sanity: based on criterion cast should be successful
        {
          itkExceptionMacro("dynamic_cast<SourceInterface*> fails, but based on component criterion it shouldn't")
        }
        //TODO: Make these connections available as inputs of the SuperElastix (filter). 

        CriteriaType typeCriteria = { { "Dimensionality", { "3" } }, { "PixelType", { "double" } } };
        if (component->MeetsCriteria(typeCriteria))
        {
          // For now, we just create the readers here.
          Reader3doubleType::Pointer reader;
          reader = Reader3doubleType::New();
          std::stringstream filename;
          //filename << "C:\\wp\\SuperElastix\\bld2\\SuperElastix-build\\bin\\Debug\\sourceimage" << readercounter << ".mhd";
          filename << "source3dimage" << readercounter << ".mhd";
          reader->SetFileName(filename.str());
          this->m_Readers3double->push_back(reader);

          //TODO which way is preferred?
          // provingSourceInterface->ConnectToOverlordSource((itk::Object*)(reader.GetPointer()));
          provingSourceInterface->ConnectToOverlordSource((itk::SmartPointer<itk::Object>) reader);

        }
        else if (component->MeetsCriteria({ { "Dimensionality", { "2" } }, { "PixelType", { "float" } } }))
        {
          // For now, we just create the readers here.
          Reader2floatType::Pointer reader;
          reader = Reader2floatType::New();
          std::stringstream filename;
          //filename << "C:\\wp\\SuperElastix\\bld2\\SuperElastix-build\\bin\\Debug\\sourceimage" << readercounter << ".mhd";
          filename << "source2dimage" << readercounter << ".mhd";
          reader->SetFileName(filename.str());
          this->m_Readers2float->push_back(reader);

          //TODO which way is preferred?
          // provingSourceInterface->ConnectToOverlordSource((itk::Object*)(reader.GetPointer()));
          provingSourceInterface->ConnectToOverlordSource((itk::SmartPointer<itk::Object>) reader);

        }
        else
        {
          itkExceptionMacro("Overlord implements only 2 float and 3 double for now");
        }
        ++readercounter;
      }
    }

    return true;
  }

  bool Overlord::ConnectSinks()
  {
    /** Scans all Components to find those with Sinking capability and store them in SinkComponents list */
    const CriterionType sinkCriterion = CriterionType("HasProvidingInterface", { "SinkInterface" });

    int writercounter = 0; // temporary solution for writer filenames

    // TODO redesign ComponentBase class to accept a single criterion instead of a criteria mapping.
    CriteriaType sinkCriteria;
    sinkCriteria.insert(sinkCriterion);

    for (auto && componentSelector : (this->m_ComponentSelectorContainer))
    {
      ComponentBase::Pointer component = componentSelector->GetComponent();
      if (component->MeetsCriteria(sinkCriteria))  // TODO MeetsCriterion
      {
        SinkInterface* provingSinkInterface = dynamic_cast<SinkInterface*> (&(*component));
        if (provingSinkInterface == nullptr) // is actually a double-check for sanity: based on criterion cast should be successful
        {
          itkExceptionMacro("dynamic_cast<SinkInterface*> fails, but based on component criterion it shouldn't")
        }

        //TODO: Make these connections available as outputs of the SuperElastix (filter). 
        if (component->MeetsCriteria({ { "Dimensionality", { "3" } }, { "PixelType", { "double" } } }))
        {
          // For now, we just create the writers here.
          Writer3doubleType::Pointer writer;
          writer = Writer3doubleType::New();
          std::stringstream filename;
          filename << "sink3dimage" << writercounter << ".mhd";
          writer->SetFileName(filename.str());
          this->m_Writers3double->push_back(writer);

          // For testing purposes, all Sources are connected to an ImageWriter
          provingSinkInterface->ConnectToOverlordSink((itk::SmartPointer<itk::Object>) writer);
        }
        else if (component->MeetsCriteria({ { "Dimensionality", { "2" } }, { "PixelType", { "float" } } }))
        {
          // For now, we just create the writers here.
          Writer2floatType::Pointer writer;
          writer = Writer2floatType::New();
          std::stringstream filename;
          filename << "sink2dimage" << writercounter << ".mhd";
          writer->SetFileName(filename.str());
          this->m_Writers2float->push_back(writer);

          // For testing purposes, all Sources are connected to an ImageWriter
          provingSinkInterface->ConnectToOverlordSink((itk::SmartPointer<itk::Object>) writer);
        }
        else
        {
          itkExceptionMacro("Overlord implements only 2 float and 3 double for now");
        }
        ++writercounter;
      }
    }

    return true;
  }

  bool Overlord::FindRunRegistration()
  {
    /** Scans all Components to find those with Sourcing capability and store them in SourceComponents list */
    const CriterionType runRegistrationCriterion = CriterionType("HasProvidingInterface", { "RunRegistrationInterface" });

    // TODO redesign ComponentBase class to accept a single criterion instead of a criteria mapping.
    CriteriaType runRegistrationCriteria;
    runRegistrationCriteria.insert(runRegistrationCriterion);

    for (auto && componentSelector : (this->m_ComponentSelectorContainer))
    {
      ComponentBase::Pointer component = componentSelector->GetComponent();
      if (component->MeetsCriteria(runRegistrationCriteria)) // TODO MeetsCriterion
      {
        this->m_RunRegistrationComponents->push_back(component);
      }
    }

    return true;
  }

  bool Overlord::RunRegistrations()
  {

    for (auto const & runRegistrationComponent : *(this->m_RunRegistrationComponents)) // auto&& preferred?
    {
      RunRegistrationInterface* provingRunRegistrationInterface = dynamic_cast<RunRegistrationInterface*> (&(*runRegistrationComponent));
      if (provingRunRegistrationInterface == nullptr) // is actually a double-check for sanity: based on criterion cast should be successful
      {
        itkExceptionMacro("dynamic_cast<RunRegistrationInterface*> fails, but based on component criterion it shouldn't")
      }
      // For testing purposes, all Sources are connected to an ImageWriter
      provingRunRegistrationInterface->RunRegistration();
    }
    return true;
  }
  bool Overlord::Execute()
  {
    
    for (auto const & reader : *(this->m_Readers2float)) // auto&& preferred?
    {
      reader->Update();
    }
    for (auto const & reader : *(this->m_Readers3double)) // auto&& preferred?
    {
      reader->Update();
    }
    // RunRegistrations is a simple execution model
    // E.g.if the components are true itk Process Object, the don't need an 'Update' call. 
    // The container of RunRegistrationsInterfaces will therefore be empty, since they will not be added if they don't expose this interface.
    // If components need RunIterations() or RunResolution() we can explicitly 'Update' them here and control the flow.
    // TODO: see if signals-and-slots paradigm is appropriate here.

    this->RunRegistrations();
    
    //update all writers...
    for (auto const & writer : *(this->m_Writers2float)) // auto&& preferred?
    {
      writer->Update();
    }
    for (auto const & writer : *(this->m_Writers3double)) // auto&& preferred?
    {
      writer->Update();
    }

    return true;
  }
} // end namespace selx

