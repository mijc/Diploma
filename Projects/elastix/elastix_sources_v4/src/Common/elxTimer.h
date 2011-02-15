/*======================================================================

  This file is part of the elastix software.

  Copyright (c) University Medical Center Utrecht. All rights reserved.
  See src/CopyrightElastix.txt or http://elastix.isi.uu.nl/legal.php for
  details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE. See the above copyright notices for more information.

======================================================================*/
#ifndef __elxTimer_H_
#define __elxTimer_H_

#include "itkObject.h"
#include "itkObjectFactory.h"
#include <ctime>
#include <sstream>

/**
 * *********************** clock() *********************************
 *
 * Return Value
 *
 * clock returns the number of clock ticks of elapsed processor time.
 * The returned value is the product of the amount of time that has
 * elapsed since the start of a process and the value of the CLOCKS_PER_SEC
 * constant. If the amount of elapsed time is unavailable, the function
 * returns -1, cast as a clock_t.
 *
 * Remarks
 *
 * The clock function tells how much processor time the calling process has
 * used. The time in seconds is approximated by dividing the clock return
 * value by the value of the CLOCKS_PER_SEC constant. In other words,
 * clock returns the number of processor timer ticks that have elapsed.
 * A timer tick is approximately equal to 1/CLOCKS_PER_SEC second. In
 * versions of Microsoft C before 6.0, the CLOCKS_PER_SEC constant
 * was called CLK_TCK.
 */

namespace tmr
{
using namespace itk;

/**
 * \class Timer
 * \brief A class to time the different parts of the registration.
 *
 * This class is a wrap around ctime.h. It is used to time the registration,
 * to get the time per iteration, or whatever.
 *
 * \ingroup Timer
 */

class Timer : public Object
{
public:
  /** Standard ITK-stuff.*/
  typedef Timer                       Self;
  typedef Object                      Superclass;
  typedef SmartPointer<Self>          Pointer;
  typedef SmartPointer<const Self>    ConstPointer;

  /** Method for creation through the object factory.*/
  itkNewMacro( Self );

  /** Run-time type information (and related methods).*/
  itkTypeMacro( Timer, Object );

  /** My typedef's.*/
  typedef std::vector<std::size_t>         TimeDHMSType;

  /** Member functions.*/
  void StartTimer( void );
  int StopTimer( void );
  int ElapsedClockAndTime( void );

  /** Formatted Output Functions
   * (return the time as a string, with comments)
   */
  const std::string & PrintStartTime( void );
  const std::string & PrintStopTime( void );
  const std::string & PrintElapsedTimeDHMS( void );
  const std::string & PrintElapsedTimeSec( void );
  const std::string & PrintElapsedClock( void );
  const std::string & PrintElapsedClockSec( void );

  /** Communication with outside world.*/
  itkGetConstMacro( StartTime, time_t );
  itkGetConstMacro( StopTime, time_t );
  itkGetConstMacro( ElapsedTime, time_t );
  //  itkGetConstMacro( ElapsedTimeDHMS, TimeDHMSType );
  itkGetConstMacro( ElapsedTimeSec, std::size_t );
  itkGetConstMacro( ElapsedClock, double );
  itkGetConstMacro( ElapsedClockSec, double );

protected:

  Timer();
  virtual ~Timer(){};

  /** Variables that store program arguments.*/
  time_t        m_StartTime;
  clock_t       m_StartClock;
  time_t        m_StopTime;
  clock_t       m_StopClock;
  double        m_ElapsedTime;
  clock_t       m_ElapsedClock;
  TimeDHMSType  m_ElapsedTimeDHMS;
  std::size_t   m_ElapsedTimeSec;
  double        m_ElapsedClockSec;

  /** Strings that serve as output of the Formatted Output Functions */
  std::string m_StartTimeString;
  std::string m_StopTimeString;
  std::string m_ElapsedTimeDHMSString;
  std::string m_ElapsedTimeSecString;
  std::string m_ElapsedClockString;
  std::string m_ElapsedClockSecString;

private:

  Timer( const Self& );           // purposely not implemented
  void operator=( const Self& );  // purposely not implemented

}; // end class Timer


} // end namespace tmr


#endif // end #ifndef __elxTimer_H_
