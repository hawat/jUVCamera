#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <string.h>
#include <jni.h>
#include "pl_hawat_jUVCamera_UVCWebCam.h"
#define __UVCWEBCAM_DEBUG
#ifdef	__cplusplus
extern "C" {
#endif

#define CLEAR(x) memset (&(x), 0, sizeof (x))    
#define THROW_EX_IF(ARG, VAL,ERR_MSG, JENV, JOBJ) if (ARG < VAL) {jex_ThrowByName(JENV,"pl/hawat/jUVCamera/UVCWebCamException",ERR_MSG); return;}
#define THROW_EX_NU(ARG, ERR_MSG,JENV, JOBJ) if (!ARG) {jex_ThrowByName(JENV,"pl/hawat/jUVCamera/UVCWebCamException",ERR_MSG); return;}
#define MJPG 000000001b;
#define YUV  000000000b;

//    
//based on MJPG-streamer
//http://sourceforge.net/projects/mjpg-streamer/
//huffner table, frames from webcam don't have this data
//we must manually insert it
//    
    const static char dht_data[] = {
    0xff, 0xc4, 0x01, 0xa2, 0x00, 0x00, 0x01, 0x05, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02,
    0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x01, 0x00, 0x03,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
    0x0a, 0x0b, 0x10, 0x00, 0x02, 0x01, 0x03, 0x03, 0x02, 0x04, 0x03, 0x05,
    0x05, 0x04, 0x04, 0x00, 0x00, 0x01, 0x7d, 0x01, 0x02, 0x03, 0x00, 0x04,
    0x11, 0x05, 0x12, 0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07, 0x22,
    0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x08, 0x23, 0x42, 0xb1, 0xc1, 0x15,
    0x52, 0xd1, 0xf0, 0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0a, 0x16, 0x17,
    0x18, 0x19, 0x1a, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x34, 0x35, 0x36,
    0x37, 0x38, 0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a,
    0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x63, 0x64, 0x65, 0x66,
    0x67, 0x68, 0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a,
    0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95,
    0x96, 0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, 0xa8,
    0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2,
    0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4, 0xd5,
    0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7,
    0xe8, 0xe9, 0xea, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9,
    0xfa, 0x11, 0x00, 0x02, 0x01, 0x02, 0x04, 0x04, 0x03, 0x04, 0x07, 0x05,
    0x04, 0x04, 0x00, 0x01, 0x02, 0x77, 0x00, 0x01, 0x02, 0x03, 0x11, 0x04,
    0x05, 0x21, 0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71, 0x13, 0x22,
    0x32, 0x81, 0x08, 0x14, 0x42, 0x91, 0xa1, 0xb1, 0xc1, 0x09, 0x23, 0x33,
    0x52, 0xf0, 0x15, 0x62, 0x72, 0xd1, 0x0a, 0x16, 0x24, 0x34, 0xe1, 0x25,
    0xf1, 0x17, 0x18, 0x19, 0x1a, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x35, 0x36,
    0x37, 0x38, 0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4a,
    0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x63, 0x64, 0x65, 0x66,
    0x67, 0x68, 0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a,
    0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8a, 0x92, 0x93, 0x94,
    0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
    0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba,
    0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4,
    0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7,
    0xe8, 0xe9, 0xea, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9, 0xfa
};

typedef struct cam_buffer {
        void *                  start;
        size_t                  length;
} cam_buffer;


typedef struct frame {
    char *buf;
    size_t size;
} frame;

    
    
struct cam_buffer * _cam_buffers = NULL;
static unsigned int _cam_n_buffers = 0;
unsigned char *bframe ;

jint JNI_OnLoad(JavaVM* vm, void* reserved) 
{
     jint result = -1;
#ifdef __UVCWEBCAM_DEBUG
     printf("__UVCWEBCAM_DEBUG:F:%s:L:%i: Loading native lib... \n",__FILE__,__LINE__);
#endif
     result = JNI_VERSION_1_4;
     return result;

}


//
//based on http://java.sun.com/docs/books/jni/html/exceptions.html#26050
//throws a exception given in <name> 
void
 jex_ThrowByName                (JNIEnv *               j_env,
                                const char *            name,
                                const char *            msg)
 {
     jclass cls = (*j_env)->FindClass(j_env, name);
     /* if cls is NULL, an exception has already been thrown */
     if (cls != NULL) {
         (*j_env)->ThrowNew(j_env, cls, msg);
     }
     /* free the local ref */
     (*j_env)->DeleteLocalRef(j_env, cls);
 }


static int
xioctl                          (int                    fd,
                                 int                    request,
                                 void *                 arg)
{
        int r;

        do r = ioctl (fd, request, arg);
        while (-1 == r && EINTR == errno);

        return r;
}

int 
jex_getJavaIntField                (JNIEnv *               j_env,
                                jobject                 j_obj,
                                char *                  field_name)
{
    jfieldID j_fid;
    jclass j_cls = (*j_env)->GetObjectClass(j_env, j_obj);
    jint j_int;
    j_fid = (*j_env)->GetFieldID(j_env, j_cls, "camera_dev_fd", "I");
    if (j_fid == NULL) 
    {  
#ifdef __UVCWEBCAM_DEBUG
     printf("__UVCWEBCAM_DEBUG:F:%s:L:%i: failed to find: field_name :%s \n",__FILE__,__LINE__,field_name);
#endif
     return 0; /* failed to find the field */
    }
    j_int = (jint)(*j_env)->GetIntField(j_env, j_obj, j_fid);
    return j_int;
}

JNIEXPORT void JNICALL Java_pl_hawat_UVCamera_UVCamera_free_1resuorces
  (JNIEnv * j_env, jobject j_obj)
{
     unsigned int i;
     for (i = 0; i < _cam_n_buffers; ++i)
     if (-1 == munmap (_cam_buffers[i].start, _cam_buffers[i].length))
        perror("Failed to free resources");
     free (_cam_buffers);
}
    
    
/*
 * Class:     pl_hawat_jUVCamera_UVCWebCam
 * Method:    stop
 * Signature: ()V
 */
JNIEXPORT void JNICALL 
Java_pl_hawat_jUVCamera_UVCWebCam_stop  (JNIEnv *       j_env,
                                        jobject         j_obj)
{
   
    int dev_file_fd = jex_getJavaIntField(j_env, j_obj,"camera_dev_fd");
    enum v4l2_buf_type v4l_type;
    v4l_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (xioctl(dev_file_fd, VIDIOC_STREAMOFF, &v4l_type)<0)
    { // "VIDIOC_STREAMOFF"
        jex_ThrowByName(j_env,"pl/hawat/jUVCamera/UVCWebCamException","Failed to stop");
    }
   
}

/*
 * Class:     pl_hawat_jUVCamera_UVCWebCam
 * Method:    start
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_pl_hawat_jUVCamera_UVCWebCam_start
  (JNIEnv *j_env, jobject j_obj)
{
        unsigned int i;
        int dev_file_fd = jex_getJavaIntField(j_env, j_obj,"camera_dev_fd");
#ifdef __UVCWEBCAM_DEBUG
        printf("__UVCWEBCAM_DEBUG:F:%s:L:%i:dev_file_fd: %i \n",__FILE__,__LINE__,dev_file_fd);
#endif
        enum v4l2_buf_type type;
        for (i = 0; i < _cam_n_buffers; ++i) {
            		struct v4l2_buffer buf;

        		CLEAR (buf);

        		buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        		buf.memory      = V4L2_MEMORY_MMAP;
        		buf.index       = i;
                        THROW_EX_IF(
                                xioctl (dev_file_fd, VIDIOC_QBUF, &buf),
                                0,
                                "VIDIOC_QBUF",
                                j_env,
                                j_obj    
                                );
		}
		type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                THROW_EX_IF(
                        xioctl (dev_file_fd, VIDIOC_STREAMON, &type),
                        0,
                        "VIDIOC_STREAMON",
                        j_env,
                        j_obj    
                        );
}

/*
 * Class:     pl_hawat_jUVCamera_UVCWebCam
 * Method:    open
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_pl_hawat_jUVCamera_UVCWebCam_open
  (JNIEnv *j_env, jobject j_obj, jstring j_path_to_file)
{
    jboolean j_iscopy;
    jint dev_desciptor = -1;
#ifdef __UVCWEBCAM_DEBUG
    printf("Java_UVCamera_open_1camera\n");
#endif
    const char * device_pth = (*j_env)->GetStringUTFChars(j_env,j_path_to_file,&j_iscopy);
#ifdef __UVCWEBCAM_DEBUG
    printf("Device to open: %s\n",device_pth);
#endif
    if (access(device_pth,O_RDWR|O_NONBLOCK)!=0)
    {         
        dev_desciptor = open(device_pth, O_RDWR /* required */ | O_NONBLOCK, 0);
 
    }// else: throw exception back to java
    else jex_ThrowByName(j_env,"pl/hawat/jUVCamera/UVCWebCamException","Failed to open device");
    (*j_env)->ReleaseStringUTFChars(j_env, j_path_to_file, device_pth);
#ifdef __UVCWEBCAM_DEBUG
     printf("__UVCWEBCAM_DEBUG:F:%s:L:%i: dev_desciptor :%i \n",__FILE__,__LINE__,dev_desciptor);
#endif
    return dev_desciptor;
}

/*
 * Class:     pl_hawat_jUVCamera_UVCWebCam
 * Method:    init
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_pl_hawat_jUVCamera_UVCWebCam_init
  (JNIEnv * j_env, jobject j_obj, jint j_height, jint j_width)
{
    
        int dev_file_fd = jex_getJavaIntField(j_env, j_obj,"camera_dev_fd");
#ifdef __UVCWEBCAM_DEBUG
         printf("__UVCWEBCAM_DEBUG:F:%s:L:%i: dev_file_fd :%i \n",__FILE__,__LINE__,dev_file_fd);
#endif
        struct v4l2_capability cap;
        struct v4l2_cropcap cropcap;
        struct v4l2_crop crop;
        struct v4l2_format fmt;
        struct v4l2_requestbuffers req;
	unsigned int min;
        THROW_EX_IF(
            xioctl (dev_file_fd, VIDIOC_QUERYCAP, &cap),
            0,
            "VIDIOC_QUERYCAP",
            j_env,
            j_obj    
            );
        THROW_EX_IF(
            (cap.capabilities & V4L2_CAP_VIDEO_CAPTURE),
            1,
            "device is not a video capture device",
            j_env,
            j_obj    
            );
        THROW_EX_IF(
            (cap.capabilities & V4L2_CAP_STREAMING),
            1,
            "device does not support streaming IO",
            j_env,
            j_obj    
            );
        CLEAR (cropcap);
        cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if (0 == xioctl (dev_file_fd, VIDIOC_CROPCAP, &cropcap)) {
                crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                crop.c = cropcap.defrect; /* reset to default */
                (void) xioctl (dev_file_fd, VIDIOC_S_CROP, &crop); /* output ignored... */
                       
                
        } 
        CLEAR (fmt);
        fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        fmt.fmt.pix.width       = j_width; 
        fmt.fmt.pix.height      = j_height;
//        fmt.fmt.pix.width       = 320; 
//        fmt.fmt.pix.height      = 240;
//        fmt.fmt.pix.width       = 752; 
//        fmt.fmt.pix.height      = 416;
        fmt.fmt.pix.pixelformat =  V4L2_PIX_FMT_MJPEG;
        fmt.fmt.pix.field       = V4L2_FIELD_INTERLACED;
        THROW_EX_IF(
            xioctl (dev_file_fd, VIDIOC_S_FMT, &fmt),
            0,
            "VIDIOC_S_FMT",
            j_env,
            j_obj    
            );
        CLEAR (req);
        req.count               = 8;
        req.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        req.memory              = V4L2_MEMORY_MMAP;
        THROW_EX_IF(
             xioctl (dev_file_fd, VIDIOC_REQBUFS, &req),
            0,
            "VIDIOC_REQBUFS",
            j_env,
            j_obj    
            );
        THROW_EX_IF(
             req.count,
            2,
            "Insufficient buffer memory",
            j_env,
            j_obj    
            );
        _cam_buffers = calloc (req.count, sizeof (*_cam_buffers));
        THROW_EX_NU(
            _cam_buffers,
            "Out of memory",
            j_env,
            j_obj    
            );
         for (_cam_n_buffers = 0; _cam_n_buffers < req.count; ++_cam_n_buffers) {
                struct v4l2_buffer buf;

                CLEAR (buf);

                buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                buf.memory      = V4L2_MEMORY_MMAP;
                buf.index       = _cam_n_buffers;
                THROW_EX_IF(
                        xioctl (dev_file_fd, VIDIOC_QUERYBUF, &buf),
                        0,
                        "VIDIOC_QUERYBUF",
            j_env,
            j_obj    
                        );
                _cam_buffers[_cam_n_buffers].length = buf.length;
                _cam_buffers[_cam_n_buffers].start =
                        mmap (NULL /* start anywhere */,
                              buf.length,
                              PROT_READ | PROT_WRITE /* required */,
                              MAP_SHARED /* recommended */,
                              dev_file_fd, buf.m.offset);

                if (MAP_FAILED == _cam_buffers[_cam_n_buffers].start)
                {
                    jex_ThrowByName(j_env,"pl/hawat/jUVCamera/UVCWebCamException","mmap failed");
                    //return on error, set return value to negative value
                    return ;
                }
                        
        }
        bframe = (unsigned char *)calloc(1, 50000);
        if (bframe==NULL) 
        {
             jex_ThrowByName(j_env,"pl/hawat/jUVCamera/UVCWebCamException","can't allocate memory for frame");
             //return on error, set return value to negative value
             return ;
        }
 
}

/*
 * Class:     pl_hawat_jUVCamera_UVCWebCam
 * Method:    captureFrame
 * Signature: ()[B
 */
JNIEXPORT jbyteArray JNICALL Java_pl_hawat_jUVCamera_UVCWebCam_captureFrame
  (JNIEnv *j_env, jobject j_obj)
{
        int dev_file_fd = jex_getJavaIntField(j_env, j_obj,"camera_dev_fd"); 
        fd_set fds;
         struct timeval tv;
         struct v4l2_buffer buf;
         unsigned int r,size= 0;
         while (size <= 619)
         {
         FD_ZERO (&fds);
         FD_SET (dev_file_fd, &fds);

         /* Timeout. */
         tv.tv_sec = 4;
         tv.tv_usec = 0;

         r = select (dev_file_fd + 1, &fds, NULL, NULL, &tv);

            if (-1 == r) {
                if (EINTR != errno) {
                    jex_ThrowByName(j_env, "pl/hawat/jUVCamera/UVCWebCamException", "select failed");
                    return;
                } else {
                    jex_ThrowByName(j_env, "pl/hawat/jUVCamera/UVCWebCamException", "Select Interrupted\n");
                }

                if (0 == r) {

                    jex_ThrowByName(j_env, "pl/hawat/jUVCamera/UVCWebCamException", "select timeout");
                    return;
                }
            }
	  CLEAR (buf);

          buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
          buf.memory = V4L2_MEMORY_MMAP;

    		if (-1 == xioctl (dev_file_fd, VIDIOC_DQBUF, &buf)) {
            		switch (errno) {
            		case EAGAIN:
                    		

			case EIO:
				/* Could ignore EIO, see spec. */

				/* fall through */

			default:{
                                jex_ThrowByName(j_env,"pl/hawat/jUVCamera/UVCWebCamException","VIDIOC_DQBUF");
                                return;
                                }
				
			}
		}
                size = buf.bytesused;

                THROW_EX_IF(
                        xioctl (dev_file_fd, VIDIOC_QBUF, &buf),
                        0,
                        "VIDIOC_QBUF",
                        j_env,
                        j_obj    
                        );
          
        }
#ifdef  __LIBSIMPLEUVC_DUMPRAWJPG
        memcpy(bframe, _cam_buffers[buf.index].start, buf.bytesused);
        int file = open("/tmp/SIMPLEUVC_RAWDUMP.jpg", O_CREAT | O_RDWR);
        if (file) {
            write(file, bframe, buf.bytesused);
            close(file);
        } else
            perror("Can`t create file!");
#endif 
        int frame_header_size = 187;
        memcpy(bframe, _cam_buffers[buf.index].start, frame_header_size);
        memcpy(bframe + frame_header_size, dht_data, 432);
        memcpy(bframe + frame_header_size + 432, _cam_buffers[buf.index].start + frame_header_size, buf.bytesused - frame_header_size);
#ifdef  __LIBSIMPLEUVC_DUMPJPG

        int filej = open("/tmp/SIMPLEUVC_DUMP.jpg", O_CREAT | O_RDWR);
        if (filej) {
            unsigned int hmn = write(filej, bframe, buf.bytesused + 432);
            printf("native: writed: %o\n", hmn);
            close(filej);
        } else
            perror("Can`t create file!");
#endif          
        jbyteArray jByArray = (jbyteArray) (*j_env)->NewByteArray(j_env, ((int) buf.bytesused) + 432);
        if (jByArray == NULL) 
        {
            return NULL; /* out of memory error thrown */
            printf("native: error, no memory...\n");
        }
        (*j_env)->SetByteArrayRegion(j_env, jByArray, 0, ((int) buf.bytesused) + 432, (jbyte *) bframe);
#ifdef __UVCWEBCAM_DEBUG        
        printf("__UVCWEBCAM_DEBUG:F:%s:L:%i:frame size: %o\n",__FILE__,__LINE__,buf.bytesused + 432);
#endif        

        return (jByArray);
}

#ifdef	__cplusplus
}
#endif
