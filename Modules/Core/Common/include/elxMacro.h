#ifndef selxMacro_h
#define selxMacro_h

/**
 * Register class with the object factory and set RTTI (Run-Time Type 
 * Information)
 */
#define selxNewMacro( className, superClassName )        \
  typedef className                       Self;         \
  typedef itk::SmartPointer< Self >       Pointer;      \
  typedef itk::SmartPointer< const Self > ConstPointer; \
  itkNewMacro( Self );                                  \
  itkTypeMacro( className, superClassName );            \

#endif // selxMacro_h