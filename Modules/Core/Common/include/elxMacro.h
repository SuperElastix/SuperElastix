#ifndef elxMacro_h
#define elxMacro_h

/**
 * Register class with the object factory and set RTTI (Run-Time Type 
 * Information)
 */
#define elxNewMacro( className, superClassName )        \
  typedef className                       Self;         \
  typedef itk::SmartPointer< Self >       Pointer;      \
  typedef itk::SmartPointer< const Self > ConstPointer; \
  itkNewMacro( Self );                                  \
  itkTypeMacro( Self, superClassName );                 \

#endif // elxMacro_h