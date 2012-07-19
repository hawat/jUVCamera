/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package juvcwebcamtest;

import java.awt.Graphics2D;
import java.awt.image.BufferedImage;
import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.imageio.ImageIO;
import javax.swing.JFrame;
import pl.hawat.jUVCamera.*;

/**
 *
 * @author hawat
 */
public class JUVCWebCamTest {

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        byte[] raw_frame;
        BufferedImage image;
        try {
            UVCWebCam webcam = new UVCWebCam("/dev/video0");
            System.out.println("Device FD:"+ Integer.toString( webcam.get_camera_dev_fd()));
            webcam.init(640, 480);
            webcam.start();
            raw_frame = webcam.captureFrame();
            webcam.stop();
            
            JFrame jframe = new JFrame();
            jframe.setSize(640, 480);
            jframe.validate();
            jframe.setVisible(true);
            jframe.setDefaultCloseOperation(javax.swing.WindowConstants.EXIT_ON_CLOSE);

            image = ImageIO.read(new ByteArrayInputStream(raw_frame));
            Graphics2D g2 = (Graphics2D) jframe.getGraphics();
            g2.drawImage(image, 0, 0, jframe.getWidth(), jframe.getHeight(), null);
            if (image != null) {
                System.out.println("Image size: " + Integer.toString(image.getHeight()) + Integer.toString(image.getWidth()));
            }
        } catch (IOException ex) {
            Logger.getLogger(JUVCWebCamTest.class.getName()).log(Level.SEVERE, null, ex);
        } catch (UVCWebCamException ex) {
            Logger.getLogger(JUVCWebCamTest.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
}
