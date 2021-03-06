/*======================================================================

This file is part of the elastix software.

Copyright (c) University Medical Center Utrecht. All rights reserved.
See src/CopyrightElastix.txt or http://elastix.isi.uu.nl/legal.php for
details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the above copyright notices for more information.

======================================================================*/

#ifndef __elxConfiguration_H__
#define __elxConfiguration_H__

#include "itkObject.h"
#include "elxBaseComponent.h"

#include "itkParameterFileParser.h"
#include "itkParameterMapInterface.h"
#include <map>
#include "xoutmain.h"


namespace elastix
{

/**
 * \class Configuration
 * \brief A class that deals with user given parameters and command line arguments.
 *
 * The Configuration class provides the functions
 * ReadParameter() (to read parameters from the parameter file) and
 * ReadCommandLineArgument(), and provides an easy way to get the
 * current elastix level.
 *
 * \parameter PrintErrorMessages: defines if warnings and errors should be
 * printed to screen, e.g. when a parameter cannot be found and the default
 * is used.
 * example: <tt>(PrintErrorMessages "false")</tt>\n
 * Default: "true"
 *
 * \ingroup Configuration
 */

class Configuration : public itk::Object, public BaseComponent
{
public:

  /** Standard itk.*/
  typedef Configuration                   Self;
  typedef itk::Object                     Superclass1;
  typedef BaseComponent                   Superclass2;
  typedef itk::SmartPointer<Self>         Pointer;
  typedef itk::SmartPointer<const Self>   ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro( Self );

  /** Standard part of all itk objects. */
  itkTypeMacro( Configuration, itk::Object );

  /** Typedefs for the command line arguments. */
  typedef std::map<std::string, std::string>      CommandLineArgumentMapType;
  typedef CommandLineArgumentMapType::value_type  CommandLineEntryType;

  /** Typedefs for the parameter file. */
  typedef itk::ParameterFileParser              ParameterFileParserType;
  typedef ParameterFileParserType::Pointer      ParameterFileParserPointer;
  typedef itk::ParameterMapInterface            ParameterMapInterfaceType;
  typedef ParameterMapInterfaceType::Pointer    ParameterMapInterfacePointer;

  /** Get and Set CommandLine arguments into the argument map. */
  const std::string GetCommandLineArgument( const std::string & key ) const;
  void SetCommandLineArgument( const std::string & key, const std::string & value );

  /** Get/Set the name of the parameterFileName. */
  itkGetStringMacro( ParameterFileName );
  itkSetStringMacro( ParameterFileName );

  /** Pass the command line arguments as a map.
   * One of the command line arguments should be either:
   * -p \<parameter_file\>         or
   * -tp \<transform_parameter_file\>.
   * The specified (transform) parameter file is read by the
   * itk::ParameterFileParser and passed to the itk::ParameterMapInterface.
   */
  virtual int Initialize( const CommandLineArgumentMapType & _arg );

  /** True, if Initialize was successfully called. */
  virtual bool IsInitialized( void ) const; //to elxconfigurationbase

  /** Other elastix related information. */

  /** Get and Set the elastix level. */
  itkSetMacro( ElastixLevel, unsigned int );
  itkGetConstMacro( ElastixLevel, unsigned int );

  /** Get and Set the total number of elastix levels. */
  itkSetMacro( TotalNumberOfElastixLevels, unsigned int );
  itkGetConstMacro( TotalNumberOfElastixLevels, unsigned int );

  /***/
  virtual bool GetPrintErrorMessages( void )
  {
    return this->m_ParameterMapInterface->GetPrintErrorMessages();
  }

  /** Set/Get whether warnings are allowed to be printed, when reading a parameter. *
  itkSetMacro( Silent, bool );
  itkGetConstMacro( Silent, bool );
  */

  /** Methods that is called at the very beginning of elastixTemplate::Run.
   * \li Prints the parameter file
   */
  virtual int BeforeAll( void );

  /** Methods that is called at the very beginning of elastixTemplate::ApplyTransform.
   * \li Prints the parameter file
   */
  virtual int BeforeAllTransformix( void );


  /** Interface to the ParameterMapInterface. */

  /** Count the number of parameters. */
  unsigned int CountNumberOfParameterEntries(
    const std::string & parameterName ) const
  {
    return this->m_ParameterMapInterface->CountNumberOfParameterEntries(
      parameterName );
  }

  /** Read a parameter from the parameter file. */
  template <class T>
  bool ReadParameter( T & parameterValue, const std::string & parameterName,
    const unsigned int entry_nr, const bool printThisErrorMessage )
  {
    std::string errorMessage = "";
    bool found = this->m_ParameterMapInterface->ReadParameter(
      parameterValue, parameterName, entry_nr,
      printThisErrorMessage, errorMessage );
    if ( errorMessage != "" )
    {
      xl::xout["error"] << errorMessage;
    }

    return found;
  }

  /** Read a parameter from the parameter file. */
  template <class T>
  bool ReadParameter( T & parameterValue, const std::string & parameterName,
    const unsigned int entry_nr )
  {
    std::string errorMessage = "";
    bool found = this->m_ParameterMapInterface->ReadParameter(
      parameterValue, parameterName, entry_nr, errorMessage );
    if ( errorMessage != "" )
    {
      xl::xout["error"] << errorMessage;
    }

    return found;
  }

  /** Read a parameter from the parameter file. */
  template <class T>
  bool ReadParameter( T & parameterValue, const std::string & parameterName,
    const std::string & prefix,
    const unsigned int entry_nr, const int default_entry_nr,
    const bool printThisErrorMessage ) const
  {
    std::string errorMessage = "";
    bool found = this->m_ParameterMapInterface->ReadParameter(
      parameterValue, parameterName, prefix, entry_nr, default_entry_nr,
      printThisErrorMessage, errorMessage );
    if ( errorMessage != "" )
    {
      xl::xout["error"] << errorMessage;
    }

    return found;
  }

  /** Read a parameter from the parameter file. */
  template <class T>
  bool ReadParameter( T & parameterValue, const std::string & parameterName,
    const std::string & prefix,
    const unsigned int entry_nr, const int default_entry_nr ) const
  {
    std::string errorMessage = "";
    bool found = this->m_ParameterMapInterface->ReadParameter(
      parameterValue, parameterName, prefix, entry_nr, default_entry_nr,
      errorMessage );
    if ( errorMessage != "" )
    {
      xl::xout["error"] << errorMessage;
    }

    return found;
  }

  /** Read a range of parameters from the parameter file. */
  template <class T>
  bool ReadParameter( std::vector<T> & parameterValues,
    const std::string & parameterName,
    const unsigned int entry_nr_start,
    const unsigned int entry_nr_end,
    const bool printThisErrorMessage ) const
  {
    std::string errorMessage = "";
    bool found = this->m_ParameterMapInterface->ReadParameter(
      parameterValues, parameterName, entry_nr_start, entry_nr_end,
      printThisErrorMessage, errorMessage );
    if ( errorMessage != "" )
    {
      xl::xout["error"] << errorMessage;
    }

    return found;
  }

  protected:

    Configuration();
    virtual ~Configuration() {};

    /** Print the parameter file to the log file. Called by BeforeAll().
     * This function is not really generic. It's just added because it needs to be
     * called by both BeforeAll and BeforeAllTransformix.
     */
    virtual void PrintParameterFile( void ) const;

  private:

    Configuration( const Self& );   // purposely not implemented
    void operator=( const Self& );  // purposely not implemented

    CommandLineArgumentMapType    m_CommandLineArgumentMap;
    std::string                   m_ParameterFileName;
    ParameterFileParserPointer    m_ParameterFileParser;
    ParameterMapInterfacePointer  m_ParameterMapInterface;

    bool                          m_IsInitialized;
    unsigned int                  m_ElastixLevel;
    unsigned int                  m_TotalNumberOfElastixLevels;

  }; // end class Configuration


} // end namespace elastix


#endif // end #ifndef __elxConfiguration_H__

