#include "Overlord.h"

namespace selx
{
  Overlord::Overlord()
  {
    //this->m_SinkComponents = SinkComponentsContainerType::New();
    //this->m_SourceComponents = SourceComponentsContainerType::New();
    this->m_RunRegistrationComponents = ComponentsContainerType::New();
    this->m_AfterRegistrationComponents = ComponentsContainerType::New();
    // temporary solution
    this->m_Readers2float = Reader2floatContainerType::New();
    this->m_Writers2float = Writer2floatContainerType::New();
    this->m_Readers3double = Reader3doubleContainerType::New();
    this->m_Writers3double = Writer3doubleContainerType::New();
    
    this->m_WritersDisplacement2float = WriterDisplacement2floatContainerType::New();
    this->m_WritersDisplacement3double = WriterDisplacement3doubleContainerType::New();
    
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
    std::cout << "Applying Component Settings" << std::endl;
    allUniqueComponents = this->UpdateSelectors();
    std::cout << "Based on Component Criteria unique components could " << (allUniqueComponents ? "" : "not ") << "be selected" << std::endl;

    std::cout << "Applying Connection Settings" << std::endl;
    this->ApplyConnectionConfiguration();
    allUniqueComponents = this->UpdateSelectors();
    std::cout << "By adding Connection Criteria unique components could " << (allUniqueComponents ? "" : "not ") << "be selected" << std::endl;

    std::cout << "Connecting Sources" << std::endl;
    this->ConnectSources();

    int numberSources2float = this->m_Readers2float->size(); // temporary solution
    if (numberSources2float > 0)
    {
      std::cout << "Found " << numberSources2float << " Source Components (2d float)" << std::endl;
    }
    int numberSources3double = this->m_Readers3double->size(); // temporary solution
    if (numberSources3double > 0)
    {
      std::cout << "Found " << numberSources3double << " Source Components (3d double)" << std::endl;
    }
    std::cout << "Connecting Sinks" << std::endl;
    this->ConnectSinks();

    int numberSinks2float = this->m_Writers2float->size(); // temporary solution
    if (numberSinks2float > 0)
    {
      std::cout << "Found " << numberSinks2float << " Sink Components (2d float)" << std::endl;
    }
    int numberSinks3double = this->m_Writers3double->size(); // temporary solution
    if (numberSinks3double > 0 )
    {
      std::cout << "Found " << numberSinks3double << " Sink Components (3d double)" << std::endl;
    }

    int numberSinksDisplacement2float = this->m_WritersDisplacement2float->size(); // temporary solution
    if (numberSinksDisplacement2float > 0)
    {
      std::cout << "Found " << numberSinksDisplacement2float << " Sink Components (Displacement 2d float)" << std::endl;
    }
    int numberSinksDisplacement3double = this->m_WritersDisplacement3double->size(); // temporary solution
    if (numberSinksDisplacement3double > 0)
    {
      std::cout << "Found " << numberSinksDisplacement3double << " Sink Components (Displacement 3d double)" << std::endl;
    }


    if (allUniqueComponents)
    {
      isSuccess = this->ConnectComponents();
    }

    std::cout << "Connecting Components: " << (isSuccess? "succeeded" : "failed") << std::endl;

    this->FindRunRegistration();
    this->FindAfterRegistration();
    return isSuccess;
  }

  bool Overlord::UpdateSelectors()
  {
    bool allUniqueComponents = true;
    Blueprint::ComponentNamesType componentNames = this->m_Blueprint->GetComponentNames();
    for (auto const & name : componentNames)
    {
      ComponentSelector::NumberOfComponentsType numberOfComponents = this->m_ComponentSelectorContainer[name]->UpdatePossibleComponents();

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
      std::cout << "blueprint node " << name << " has selected " << numberOfComponents << " components" << std::endl;

    }
    return allUniqueComponents;
  }

  void Overlord::ApplyNodeConfiguration()
  {
    Blueprint::ComponentNamesType componentNames = this->m_Blueprint->GetComponentNames();
    for (auto const & name : componentNames)
    {
      ComponentSelectorPointer currentComponentSelector = ComponentSelector::New();
      Blueprint::ParameterMapType currentProperty = this->m_Blueprint->GetComponent(name);
      currentComponentSelector->SetCriteria(currentProperty);
      // insert new element
      this->m_ComponentSelectorContainer[name]=currentComponentSelector;
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

    Blueprint::ComponentNamesType componentNames = this->m_Blueprint->GetComponentNames();
    for (auto const & name : componentNames)
    {
      for (auto const & outgoingName : this->m_Blueprint->GetOutputNames(name))
      {
        //TODO check direction upstream/downstream input/output source/target
        Blueprint::ParameterMapType connectionProperties = this->m_Blueprint->GetConnection(name, outgoingName);
        if (connectionProperties.count("NameOfInterface") > 0)
        {
          ComponentBase::CriteriaType additionalSourceCriteria;
          additionalSourceCriteria.insert(ComponentBase::CriterionType("HasProvidingInterface", connectionProperties["NameOfInterface"]));

          ComponentBase::CriteriaType additionalTargetCriteria;
          additionalTargetCriteria.insert(ComponentBase::CriterionType("HasAcceptingInterface", connectionProperties["NameOfInterface"]));

          this->m_ComponentSelectorContainer[name]->AddCriteria(additionalSourceCriteria);
          this->m_ComponentSelectorContainer[outgoingName]->AddCriteria(additionalTargetCriteria);
        }
      }
    }

    return;
  }
  bool Overlord::ConnectComponents()
  {
    bool isAllSuccess = true;

    Blueprint::ComponentNamesType componentNames = this->m_Blueprint->GetComponentNames();
    for (auto const & name : componentNames)
    {
      for (auto const & outgoingName : this->m_Blueprint->GetOutputNames(name))
      {
        //TODO check direction upstream/downstream input/output source/target
        //TODO GetComponent returns NULL if possible components !=1 we can check for that, but Overlord::UpdateSelectors() does something similar.
        ComponentBase::Pointer sourceComponent = this->m_ComponentSelectorContainer[name]->GetComponent();
        ComponentBase::Pointer targetComponent = this->m_ComponentSelectorContainer[outgoingName]->GetComponent();

        Blueprint::ParameterMapType connectionProperties = this->m_Blueprint->GetConnection(name, outgoingName);
        int numberOfConnections = 0;
        if (connectionProperties.count("NameOfInterface") > 0)
        {
          // connect only via interfaces provided by user configuration
          for (auto const & interfaceName : connectionProperties["NameOfInterface"])
          {
            numberOfConnections += targetComponent->AcceptConnectionFrom(interfaceName.c_str(), sourceComponent);
          }
        }
        else
        {
          // connect via all possible interfaces
          numberOfConnections = targetComponent->AcceptConnectionFrom(sourceComponent);
        }

        if (numberOfConnections == 0)
        {
          isAllSuccess = false;
          std::cout << "Warning: a connection was specified, but no compatible interfaces were found.";
        }
      }
    }
    return isAllSuccess;
  }
  bool Overlord::ConnectSources()
  {
    /** Scans all Components to find those with Sourcing capability and store them in SourceComponents list */

    int readercounter = 0; // temporary solution for reader filenames

    // TODO redesign ComponentBase class to accept a single criterion instead of a criteria mapping.
    for (const auto & componentSelector : (this->m_ComponentSelectorContainer))
    {
      ComponentBase::Pointer component = componentSelector.second->GetComponent();

      if (component->MeetsCriteria({ { "HasProvidingInterface", { "SourceInterface" } } })) // TODO MeetsCriterion
      {
        SourceInterface* provingSourceInterface = dynamic_cast<SourceInterface*> (&(*component));
        if (provingSourceInterface == nullptr) // is actually a double-check for sanity: based on criterion cast should be successful
        {
          itkExceptionMacro("dynamic_cast<SourceInterface*> fails, but based on component criterion it shouldn't")
        }
        //TODO: Make these connections available as inputs of the SuperElastix (filter). 


        if (component->MeetsCriteria({ { "Dimensionality", { "3" } }, { "PixelType", { "double" } } }))
        {
          // For now, we just create the readers here.
          Reader3doubleType::Pointer reader;
          reader = Reader3doubleType::New();

          if (readercounter >= this->inputFileNames.size())
          {
            itkExceptionMacro("not enough inputFileNames provided")
          }

          reader->SetFileName(this->inputFileNames[readercounter]);
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

          if (readercounter >= this->inputFileNames.size())
          {
            itkExceptionMacro("not enough inputFileNames provided")
          }

          reader->SetFileName(this->inputFileNames[readercounter]);
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

    for (auto const & componentSelector : (this->m_ComponentSelectorContainer))
    {
      ComponentBase::Pointer component = componentSelector.second->GetComponent();
      if (component->MeetsCriteria(sinkCriteria))  // TODO MeetsCriterion
      {
        SinkInterface* provingSinkInterface = dynamic_cast<SinkInterface*> (&(*component));
        if (provingSinkInterface == nullptr) // is actually a double-check for sanity: based on criterion cast should be successful
        {
          itkExceptionMacro("dynamic_cast<SinkInterface*> fails, but based on component criterion it shouldn't")
        }

        //TODO: Make these connections available as outputs of the SuperElastix (filter). 
        if (component->MeetsCriteria({ { "IsVectorField", { "True" } }, { "Dimensionality", { "3" } }, { "PixelType", { "double" } } }))
        {
          // For now, we just create the readers here.
          WriterDisplacement3doubleType::Pointer writer;
          writer = WriterDisplacement3doubleType::New();

          if (writercounter >= this->inputFileNames.size())
          {
            itkExceptionMacro("not enough outputFileNames provided")
          }

          writer->SetFileName(this->outputFileNames[writercounter]);
          this->m_WritersDisplacement3double->push_back(writer);

          provingSinkInterface->ConnectToOverlordSink((itk::SmartPointer<itk::Object>) writer);
        }
        else if (component->MeetsCriteria({ { "IsVectorField", { "True" } }, { "Dimensionality", { "2" } }, { "PixelType", { "float" } } }))
        {
          // For now, we just create the readers here.
          WriterDisplacement2floatType::Pointer writer;
          writer = WriterDisplacement2floatType::New();

          if (writercounter >= this->inputFileNames.size())
          {
            itkExceptionMacro("not enough inputFileNames provided")
          }

          writer->SetFileName(this->outputFileNames[writercounter]);
          this->m_WritersDisplacement2float->push_back(writer);

          provingSinkInterface->ConnectToOverlordSink((itk::SmartPointer<itk::Object>) writer);
        }
        else if (component->MeetsCriteria({ { "Dimensionality", { "3" } }, { "PixelType", { "double" } } }))
        {
          // For now, we just create the writers here.
          Writer3doubleType::Pointer writer;
          writer = Writer3doubleType::New();

          if (writercounter >= this->outputFileNames.size())
          {
            itkExceptionMacro("not enough outputFileNames provided")
          }

          writer->SetFileName(this->outputFileNames[writercounter]);
          this->m_Writers3double->push_back(writer);

          // For testing purposes, all Sources are connected to an ImageWriter
          provingSinkInterface->ConnectToOverlordSink((itk::SmartPointer<itk::Object>) writer);
        }
        else if (component->MeetsCriteria({ { "Dimensionality", { "2" } }, { "PixelType", { "float" } } }))
        {
          // For now, we just create the writers here.
          Writer2floatType::Pointer writer;
          writer = Writer2floatType::New();

          if (writercounter  >= this->outputFileNames.size())
          {
            itkExceptionMacro("not enough outputFileNames provided")
          }
          writer->SetFileName(this->outputFileNames[writercounter]);
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

    for (auto const & componentSelector : (this->m_ComponentSelectorContainer))
    {
      ComponentBase::Pointer component = componentSelector.second->GetComponent();
      if (component->MeetsCriteria(runRegistrationCriteria)) // TODO MeetsCriterion
      {
        this->m_RunRegistrationComponents->push_back(component);
      }
    }

    return true;
  }

  bool Overlord::FindAfterRegistration()
  {
    /** Scans all Components to find those with Sourcing capability and store them in SourceComponents list */
    const CriterionType afterRegistrationCriterion = CriterionType("HasProvidingInterface", { "AfterRegistrationInterface" });

    // TODO redesign ComponentBase class to accept a single criterion instead of a criteria mapping.
    CriteriaType afterRegistrationCriteria;
    afterRegistrationCriteria.insert(afterRegistrationCriterion);

    for (auto const & componentSelector : (this->m_ComponentSelectorContainer))
    {
      ComponentBase::Pointer component = componentSelector.second->GetComponent();
      if (component->MeetsCriteria(afterRegistrationCriteria)) // TODO MeetsCriterion
      {
        this->m_AfterRegistrationComponents->push_back(component);
      }
    }

    return true;
  }

  

  bool Overlord::RunRegistrations()
  {

    for (auto const & runRegistrationComponent : *(this->m_RunRegistrationComponents)) // auto&& preferred?
    {
      RunRegistrationInterface* providingRunRegistrationInterface = dynamic_cast<RunRegistrationInterface*> (&(*runRegistrationComponent));
      if (providingRunRegistrationInterface == nullptr) // is actually a double-check for sanity: based on criterion cast should be successful
      {
        itkExceptionMacro("dynamic_cast<RunRegistrationInterface*> fails, but based on component criterion it shouldn't")
      }
      // For testing purposes, all Sources are connected to an ImageWriter
      providingRunRegistrationInterface->RunRegistration();
    }
    return true;
  }

  bool Overlord::AfterRegistrations()
  {

    for (auto const & afterRegistrationComponent : *(this->m_AfterRegistrationComponents)) // auto&& preferred?
    {
      AfterRegistrationInterface* providingAfterRegistrationInterface = dynamic_cast<AfterRegistrationInterface*> (&(*afterRegistrationComponent));
      if (providingAfterRegistrationInterface == nullptr) // is actually a double-check for sanity: based on criterion cast should be successful
      {
        itkExceptionMacro("dynamic_cast<AfterRegistrationInterface*> fails, but based on component criterion it shouldn't")
      }
      // For testing purposes, all Sources are connected to an ImageWriter
      providingAfterRegistrationInterface->AfterRegistration();
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
    this->AfterRegistrations();
    //update all writers...
    for (auto const & writer : *(this->m_Writers2float)) // auto&& preferred?
    {
      writer->Update();
    }
    for (auto const & writer : *(this->m_Writers3double)) // auto&& preferred?
    {
      writer->Update();
    }
    for (auto const & writer : *(this->m_WritersDisplacement3double)) // auto&& preferred?
    {
      writer->Update();
    }
    for (auto const & writer : *(this->m_WritersDisplacement2float)) // auto&& preferred?
    {
      writer->Update();
    }
    return true;
  }
} // end namespace selx

