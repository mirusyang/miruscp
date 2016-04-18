/*!
  \brief  
  X-Keyboard & Mouse Hook App based on the Win32 APIs.

  \file 
  xkbdutil.h

  \author 

  \date 
  2016/04/13

  \version  
  1.0

  \copyright  
  GPL

  \remarks  

*/
#ifndef _XKBDUTIL_H_
#define _XKBDUTIL_H_

#include "xkit/xkitconf.h"
#include <cstdint>

XK_CLINKAGE_BEGIN

// NOTE: functions exported here.

XK_CLINKAGE_END

XK_NAMESPACE_BEGIN

struct KbdModInterface {
  virtual ~KbdModInterface();

  /*!
    \brief  
    Initialise the mod object.
  */
  virtual bool Initialise() = 0;

  /*!
    \brief  
    Some cleanup job(s) done here. You can just ignore it or manually called it.
  */
  virtual void Release() = 0;

  /*!
    \brief  
    Maps one key to another. 
  */
  virtual void Map(uint16_t, uint16_t) = 0;

  /*!
    \brief  
    Enables the function(s) or not. Specified by the parameter \c enabled.
  */
  virtual void Enable(bool enabled) = 0;

  /*!
    \brief  
    Checks if the function(s) was enabled or not.
  */
  virtual bool Enabled() const = 0;
};

XK_NAMESPACE_END

typedef xk::KbdModInterface* (*xkGetModifierApi)(unsigned long);


#ifdef XK_MSC
# ifdef _XK_AUTOLINK_DLL
#   ifdef XK_DEBUG
#pragma comment(lib, "xkbdutil_d.lib")
#   else
#pragma comment(lib, "xkbdutil.lib")
#   endif
# elif defined(_XK_AUTOLINK_LIB)
#   ifdef XK_DEBUG
#pragma comment(lib, "libxkbdutil_d.lib")
#   else
#pragma comment(lib, "libxkbdutil.lib")
#   endif
# endif
#endif

#endif 
