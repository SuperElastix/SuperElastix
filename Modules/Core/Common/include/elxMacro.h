#ifndef __elxMacro_h
#define __elxMacro_h

/**
 * Register class with the object factory and provide 
 * associated RTTI (Run-Time Type Information)
 */
#define elxNewMacro( className, superClassName )        \
  typedef className                       Self;         \
  typedef itk::SmartPointer< Self >       Pointer;      \
  typedef itk::SmartPointer< const Self > ConstPointer; \
  itkNewMacro( Self );                                  \
  itkTypeMacro( Self, superClassName );                 \

#endif // __elxMacro_h