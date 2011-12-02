/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package nl.lemval.nododue.component;

import java.awt.Graphics;
import java.awt.image.BufferedImage;
import java.io.IOException;
import java.io.InputStream;
import javax.imageio.ImageIO;
import javax.swing.JPanel;
import nl.lemval.nododue.NodoDueManager;

/**
 *
 * @author Michael
 */
public class ArduinoImagePanel extends JPanel {

    BufferedImage image = null;

    public ArduinoImagePanel() {
        try {
            String imgPath = "/nl/lemval/nododue/resources/arduino.png";
            InputStream input = getClass().getResourceAsStream(imgPath);
            image = ImageIO.read(input);
        } catch (IOException ie) {
            NodoDueManager.showMessage("GUI.image_panel.ctor", ie.getMessage());
        }
    }

    @Override
    public void paint(Graphics g) {
        g.drawImage(image, 0, 0, null);
    }
}
