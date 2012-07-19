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
 * Simple java class 
 * 
 * 
 * 2012/07
 * @author Piotr hawat Kostrzewa
 * @version 0.1b
 */
public class UVCWebCamException extends Exception {
    private String p_message;
    
    /**
     * Constructor 
     * @param message messege from the native library 
     */
    public UVCWebCamException(String message)
    {
        p_message = message;
    }
    
    public String toString()
    {
        return p_message;
    }
    
}
