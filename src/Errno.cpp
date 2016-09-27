/*
 * Event.cpp
 *
 *      Copyright (C) 2015 Overkiz SA.
 */

#include "Errno.h"

#define ERRNO_ID_PREFIX     "com.kizbox.Framework.Core.Errno."

namespace Overkiz
{

  namespace Errno
  {

    static const char * ERRNO_ID[] =
    {
      ERRNO_ID_PREFIX"NoError", //!<
      ERRNO_ID_PREFIX"OperationNotPermitted", //!< EPERM (  1) - Operation not permitted
      ERRNO_ID_PREFIX"NoSuchFileOrDirectory", //!< ENOENT (  2) - No such file or directory
      ERRNO_ID_PREFIX"NoSuchProcess", //!< ESRCH (  3) - No such process
      ERRNO_ID_PREFIX"InterruptedSystemCall", //!< EINTR (  4) - Interrupted system call
      ERRNO_ID_PREFIX"IOError", //!< EIO (  5) - I/O error
      ERRNO_ID_PREFIX"NoSuchDeviceOrAddress", //!< ENXIO (  6) - No such device or address
      ERRNO_ID_PREFIX"ArgumentListTooLong", //!< E2BIG (  7) - Argument list too long
      ERRNO_ID_PREFIX"ExecFormatError", //!< ENOEXEC (  8) - Exec format error
      ERRNO_ID_PREFIX"BadFileNumber", //!< EBADF (  9) - Bad file number
      ERRNO_ID_PREFIX"NoChildProcesses", //!< ECHILD ( 10) - No child processes
      ERRNO_ID_PREFIX"TryAgain", //!< EAGAIN ( 11) - Try again
      ERRNO_ID_PREFIX"OutOfMemory", //!< ENOMEM ( 12) - Out of memory
      ERRNO_ID_PREFIX"PermissionDenied", //!< EACCES ( 13) - Permission denied
      ERRNO_ID_PREFIX"BadAddress", //!< EFAULT ( 14) - Bad address
      ERRNO_ID_PREFIX"BlockDeviceRequired", //!< ENOTBLK ( 15) - Block device required
      ERRNO_ID_PREFIX"DeviceOrResourceBusy", //!< EBUSY ( 16) - Device or resource busy
      ERRNO_ID_PREFIX"FileExists", //!< EEXIST ( 17) - File exists
      ERRNO_ID_PREFIX"CrossDeviceLink", //!< EXDEV ( 18) - Cross-device link
      ERRNO_ID_PREFIX"NoSuchDevice", //!< ENODEV ( 19) - No such device
      ERRNO_ID_PREFIX"NotADirectory", //!< ENOTDIR ( 20) - Not a directory
      ERRNO_ID_PREFIX"IsADirectory", //!< EISDIR ( 21) - Is a directory
      ERRNO_ID_PREFIX"InvalidArgument", //!< EINVAL ( 22) - Invalid argument
      ERRNO_ID_PREFIX"FileTableOverflow", //!< ENFILE ( 23) - File table overflow
      ERRNO_ID_PREFIX"TooManyOpenFiles", //!< EMFILE ( 24) - Too many open files
      ERRNO_ID_PREFIX"NotATypewriter", //!< ENOTTY ( 25) - Not a typewriter
      ERRNO_ID_PREFIX"TextFileBusy", //!< ETXTBSY ( 26) - Text file busy
      ERRNO_ID_PREFIX"FileTooLarge", //!< EFBIG ( 27) - File too large
      ERRNO_ID_PREFIX"NoSpaceLeftOnDevice", //!< ENOSPC ( 28) - No space left on device
      ERRNO_ID_PREFIX"IllegalSeek", //!< ESPIPE ( 29) - Illegal seek
      ERRNO_ID_PREFIX"ReadOnlyFileSystem", //!< EROFS ( 30) - Read-only file system
      ERRNO_ID_PREFIX"TooManyLinks", //!< EMLINK ( 31) - Too many links
      ERRNO_ID_PREFIX"BrokenPipe", //!< EPIPE ( 32) - Broken pipe
      ERRNO_ID_PREFIX"MathArgumentOutOfDomainOfFunc", //!< EDOM ( 33) - Math argument out of domain of func
      ERRNO_ID_PREFIX"Math result not representable", //!< ERANGE ( 34) - Math result not representable
      ERRNO_ID_PREFIX"ResourceDeadlockWouldOccur", //!< EDEADLK ( 35) - Resource deadlock would occur
      ERRNO_ID_PREFIX"FileNameTooLong", //!< ENAMETOOLONG ( 36) - File name too long
      ERRNO_ID_PREFIX"NoRecordLocksAvailable", //!< ENOLCK ( 37) - No record locks available
      ERRNO_ID_PREFIX"FunctionNotImplemented", //!< ENOSYS ( 38) - Function not implemented
      ERRNO_ID_PREFIX"DirectoryNotEmpty", //!< ENOTEMPTY ( 39) - Directory not empty
      ERRNO_ID_PREFIX"TooManySymbolicLinksEncountered", //!< ELOOP ( 40) - Too many symbolic links encountered
      ERRNO_ID_PREFIX"NoMessageOfDesiredType", //!< ENOMSG ( 42) - No message of desired type
      ERRNO_ID_PREFIX"IdentifierRemoved", //!< EIDRM ( 43) - Identifier removed
      ERRNO_ID_PREFIX"ChannelNumberOutOfRange", //!< ECHRNG ( 44) - Channel number out of range
      ERRNO_ID_PREFIX"Level2NotSynchronized", //!< EL2NSYNC ( 45) - Level 2 not synchronized
      ERRNO_ID_PREFIX"Level3Halted", //!< EL3HLT ( 46) - Level 3 halted
      ERRNO_ID_PREFIX"Level3Reset", //!< EL3RST ( 47) - Level 3 reset
      ERRNO_ID_PREFIX"LinkNumberOutOfRange", //!< ELNRNG ( 48) - Link number out of range
      ERRNO_ID_PREFIX"ProtocolDriverNotAttached", //!< EUNATCH ( 49) - Protocol driver not attached
      ERRNO_ID_PREFIX"NoCSIStructureAvailable", //!< ENOCSI ( 50) - No CSI structure available
      ERRNO_ID_PREFIX"Level2Halted", //!< EL2HLT ( 51) - Level 2 halted
      ERRNO_ID_PREFIX"InvalidExchange", //!< EBADE ( 52) - Invalid exchange
      ERRNO_ID_PREFIX"InvalidRequestDescriptor", //!< EBADR ( 53) - Invalid request descriptor
      ERRNO_ID_PREFIX"ExchangeFull", //!< EXFULL ( 54) - Exchange full
      ERRNO_ID_PREFIX"NoAnode", //!< ENOANO ( 55) - No anode
      ERRNO_ID_PREFIX"InvalidRequestCode", //!< EBADRQC ( 56) - Invalid request code
      ERRNO_ID_PREFIX"InvalidSlot", //!< EBADSLT ( 57) - Invalid slot
      ERRNO_ID_PREFIX"BadFontFileFormat", //!< EBFONT ( 59) - Bad font file format
      ERRNO_ID_PREFIX"DeviceNotAStream", //!< ENOSTR ( 60) - Device not a stream
      ERRNO_ID_PREFIX"NoDataAvailable", //!< ENODATA ( 61) - No data available
      ERRNO_ID_PREFIX"TimerExpired", //!< ETIME ( 62) - Timer expired
      ERRNO_ID_PREFIX"OutOfStreamsResources", //!< ENOSR ( 63) - Out of streams resources
      ERRNO_ID_PREFIX"MachineIsNotOnTheNetwork", //!< ENONET ( 64) - Machine is not on the network
      ERRNO_ID_PREFIX"PackageNotInstalled", //!< ENOPKG ( 65) - Package not installed
      ERRNO_ID_PREFIX"ObjectIsRemote", //!< EREMOTE ( 66) - Object is remote
      ERRNO_ID_PREFIX"LinkHasBeenSevered", //!< ENOLINK ( 67) - Link has been severed
      ERRNO_ID_PREFIX"AdvertiseError", //!< EADV ( 68) - Advertise error
      ERRNO_ID_PREFIX"SrmountError", //!< ESRMNT ( 69) - Srmount error
      ERRNO_ID_PREFIX"CommunicationErrorOnSend", //!< ECOMM ( 70) - Communication error on send
      ERRNO_ID_PREFIX"ProtocolError", //!< EPROTO ( 71) - Protocol error
      ERRNO_ID_PREFIX"MultihopAttempted", //!< EMULTIHOP ( 72) - Multihop attempted
      ERRNO_ID_PREFIX"RFSSpecificError", //!< EDOTDOT ( 73) - RFS specific error
      ERRNO_ID_PREFIX"NotADataMessage", //!< EBADMSG ( 74) - Not a data message
      ERRNO_ID_PREFIX"ValueTooLargeForDefinedDataType", //!< EOVERFLOW ( 75) - Value too large for defined data type
      ERRNO_ID_PREFIX"NameNotUniqueOnNetwork", //!< ENOTUNIQ ( 76) - Name not unique on network
      ERRNO_ID_PREFIX"FileDescriptorInBadState", //!< EBADFD ( 77) - File descriptor in bad state
      ERRNO_ID_PREFIX"RemoteAddressChanged", //!< EREMCHG ( 78) - Remote address changed
      ERRNO_ID_PREFIX"CanNotAccessANeededSharedLibrary", //!< ELIBACC ( 79) - Can not access a needed shared library
      ERRNO_ID_PREFIX"AccessingACorruptedSharedLibrary", //!< ELIBBAD ( 80) - Accessing a corrupted shared library
      ERRNO_ID_PREFIX"DotlibSectionInADotOutCorrupted", //!< ELIBSCN ( 81) - .lib section in a.out corrupted
      ERRNO_ID_PREFIX"AttemptingToLinkInTooManySharedLibraries", //!< ELIBMAX ( 82) - Attempting to link in too many shared libraries
      ERRNO_ID_PREFIX"CannotExecASharedLibraryDirectly", //!< ELIBEXEC ( 83) - Cannot exec a shared library directly
      ERRNO_ID_PREFIX"IllegalByteSequence", //!< EILSEQ ( 84) - Illegal byte sequence
      ERRNO_ID_PREFIX"InterruptedSystemCallShouldBeRestarted", //!< ERESTART ( 85) - Interrupted system call should be restarted
      ERRNO_ID_PREFIX"StreamsPipeError", //!< ESTRPIPE ( 86) - Streams pipe error
      ERRNO_ID_PREFIX"TooManyUsers", //!< EUSERS ( 87) - Too many users
      ERRNO_ID_PREFIX"SocketOperationOnNonSocket", //!< ENOTSOCK ( 88) - Socket operation on non-socket
      ERRNO_ID_PREFIX"DestinationAddressRequired", //!< EDESTADDRREQ ( 89) - Destination address required
      ERRNO_ID_PREFIX"MessageTooLong", //!< EMSGSIZE ( 90) - Message too long
      ERRNO_ID_PREFIX"ProtocolWrongTypeForSocket", //!< EPROTOTYPE ( 91) - Protocol wrong type for socket
      ERRNO_ID_PREFIX"ProtocolNotAvailable", //!< ENOPROTOOPT ( 92) - Protocol not available
      ERRNO_ID_PREFIX"ProtocolNotSupported", //!< EPROTONOSUPPORT ( 93) - Protocol not supported
      ERRNO_ID_PREFIX"SocketTypeNotSupported", //!< ESOCKTNOSUPPORT ( 94) - Socket type not supported
      ERRNO_ID_PREFIX"OperationNotSupportedOnTransportEndpoint", //!< EOPNOTSUPP ( 95) - Operation not supported on transport endpoint
      ERRNO_ID_PREFIX"ProtocolFamilyNotSupported", //!< EPFNOSUPPORT ( 96) - Protocol family not supported
      ERRNO_ID_PREFIX"AddressFamilyNotSupportedByProtocol", //!< EAFNOSUPPORT ( 97) - Address family not supported by protocol
      ERRNO_ID_PREFIX"AddressAlreadyInUse", //!< EADDRINUSE ( 98) - Address already in use
      ERRNO_ID_PREFIX"CannotAssignRequestedAddress", //!< EADDRNOTAVAIL ( 99) - Cannot assign requested address
      ERRNO_ID_PREFIX"NetworkIsDown", //!< ENETDOWN (100) - Network is down
      ERRNO_ID_PREFIX"NetworkIsUnreachable", //!< ENETUNREACH (101) - Network is unreachable
      ERRNO_ID_PREFIX"NetworkDroppedConnectionBecauseOfReset", //!< ENETRESET  (102) - Network dropped connection because of reset
      ERRNO_ID_PREFIX"SoftwareCausedConnectionAbort", //!< ECONNABORTED (103) - Software caused connection abort
      ERRNO_ID_PREFIX"ConnectionResetByPeer", //!< ECONNRESET (104) - Connection reset by peer
      ERRNO_ID_PREFIX"NoBufferSpaceAvailable", //!< ENOBUFS (105) - No buffer space available
      ERRNO_ID_PREFIX"TransportEndpointIsAlreadyConnected", //!< EISCONN (106) - Transport endpoint is already connected
      ERRNO_ID_PREFIX"TransportEndpointIsNotConnected", //!< ENOTCONN (107) - Transport endpoint is not connected
      ERRNO_ID_PREFIX"CannotSendAfterTransportEndpointShutdown", //!< ESHUTDOWN (108) - Cannot send after transport endpoint shutdown
      ERRNO_ID_PREFIX"TooManyReferencesCannotSplice", //!< ETOOMANYREFS (109) - Too many references: cannot splice
      ERRNO_ID_PREFIX"ConnectionTimedOut", //!< ETIMEDOUT (110) - Connection timed out
      ERRNO_ID_PREFIX"ConnectionRefused", //!< ECONNREFUSED (111) - Connection refused
      ERRNO_ID_PREFIX"HostIsDown", //!< EHOSTDOWN (112) - Host is down
      ERRNO_ID_PREFIX"NoRouteToHost", //!< EHOSTUNREACH (113) - No route to host
      ERRNO_ID_PREFIX"OperationAlreadyInProgress", //!< EALREADY (114) - Operation already in progress
      ERRNO_ID_PREFIX"OperationNowInProgress", //!< EINPROGRESS (115) - Operation now in progress
      ERRNO_ID_PREFIX"StaleNFSFileHandle", //!< ESTALE (116) - Stale NFS file handle
      ERRNO_ID_PREFIX"StructureNeedsCleaning", //!< EUCLEAN (117) - Structure needs cleaning
      ERRNO_ID_PREFIX"NotAXENIXNamedYypeFile", //!< ENOTNAM (118) - Not a XENIX named type file
      ERRNO_ID_PREFIX"NoXENIXSemaphoresAvailable", //!< ENAVAIL (119) - No XENIX semaphores available
      ERRNO_ID_PREFIX"IsANamedTypeFile", //!< EISNAM (120) - Is a named type file
      ERRNO_ID_PREFIX"RemoteIOError", //!< EREMOTEIO (121) - Remote I/O error
      ERRNO_ID_PREFIX"QuotaExceeded", //!< EDQUOT (122) - Quota exceeded
      ERRNO_ID_PREFIX"NoMediumFound", //!< ENOMEDIUM (123) - No medium found
      ERRNO_ID_PREFIX"WrongMediumType", //!< EMEDIUMTYPE (124) - Wrong medium type
      ERRNO_ID_PREFIX"OperationCanceled", //!< ECANCELED (125) - Operation Canceled
      ERRNO_ID_PREFIX"RequiredKeyNotAvailable", //!< ENOKEY (126) - Required key not available
      ERRNO_ID_PREFIX"KeyHasExpired", //!< EKEYEXPIRED (127) - Key has expired
      ERRNO_ID_PREFIX"KeyHasBeenRevoked", //!< EKEYREVOKED (128) - Key has been revoked
      ERRNO_ID_PREFIX"KeyWasRejectedByService", //!< EKEYREJECTED (129) - Key was rejected by service
      ERRNO_ID_PREFIX"OwnerDied", //!< EOWNERDEAD (130) - Owner died
      ERRNO_ID_PREFIX"StateNotRecoverable", //!< ENOTRECOVERABLE (131) - State not recoverable
      ERRNO_ID_PREFIX"OperationNotPossibleDueYoRFKill", //!< ERFKILL (132) - Operation not possible due to RF-kill
    };

    Exception::Exception(int error) :
      code(error)
    {
    }

    Exception::~Exception()
    {
    }

    int Exception::getErrno() const
    {
      return errno;
    }

    const char * Exception::getId() const
    {
      if(code <= (int)(sizeof(ERRNO_ID) / sizeof(const char *)) - 1)
      {
        return ERRNO_ID[code];
      }
      else
      {
        return ERRNO_ID_PREFIX"UnknowError";
      }
    }

    const char * Exception::getString(const int error)
    {
      return ERRNO_ID[error];
    }

  }

}
