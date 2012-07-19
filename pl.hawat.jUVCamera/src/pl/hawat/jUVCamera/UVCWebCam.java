/**
 *  This is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This Library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 */
package pl.hawat.jUVCamera;

/**
 * 2012/07
 * @author Piotr hawat Kostrzewa
 * @version 0.1b
 */
public class UVCWebCam {
    static {
        System.loadLibrary("jUVCWebCam");
    }
    

    private int camera_dev_fd;

     /**
     * Returns a int value of the filedesciptor opened for the video device
     * 
     * @return      filedesciptor for the opened device
     */
    public int get_camera_dev_fd()
    {
        return camera_dev_fd;
    }
    
    /**
     * Native method, will stop recording on opened video device 
     */
    public native void
                        stop();
    
    /**
     * Native method, will start recording on opened video device
     */
    public native void
                        start();
    /**
     * Native method, will open given video device (/dev/video*),
     * must be called before init()
     * 
     * @param path_to_device    path to choosen video device
     * @return                  filedesciptor of the opened device file
     * @throws pl.hawat.jUVCamera.UVCWebCamException 
     */
    private native int
        open(String path_to_device) throws pl.hawat.jUVCamera.UVCWebCamException;
    
    /**
     * Configures opened video device for given height nad width in pixels,
     * must be called before start() and captureFrame()
     * 
     * @param height
     * @param width
     * @throws pl.hawat.jUVCamera.UVCWebCamException 
     */
    public native void
        init(int height, int width) throws  pl.hawat.jUVCamera.UVCWebCamException;
    
    /**
     * Returns raw jpg frame from the video device
     * 
     * @return  raw byte array containig jpg 
     */
    public native byte[]
        captureFrame();
    
    /**
     * Constructor
     * 
     * @param path_to_device    path to the device file 
     * @throws pl.hawat.jUVCamera.UVCWebCamException 
     */
    public UVCWebCam(String path_to_device)throws pl.hawat.jUVCamera.UVCWebCamException
    {
        camera_dev_fd=open(path_to_device);
    }
    
    
    
}
