/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package nl.lemval.nododue.component;

import java.awt.Color;
import java.awt.Graphics;
import java.util.ArrayList;
import javax.swing.JPanel;
import nl.lemval.nododue.util.Wire;
import nl.lemval.nododue.util.WireData;

/**
 *
 * @author Michael
 */
public class WireGraphPanel extends JPanel {

    private Wire wire = null;
    private int highlightState = 0;
    private static final Color valueColor = Color.BLUE;
    private static final Color thresholdColor = Color.RED;
    private static final Color DEFAULT_BACKGROUND = new Color(210,220,243);
    private static final Color HIGHLIGHT_BACKGROUND = new Color(240,245,255);
    private static final Color marginColor = new Color(200, 100, 0);

    public WireGraphPanel() {
    }

    @Override
    public void paint(Graphics g) {
        super.paint(g);

        drawHighlight(g);
        
        if (wire == null) {
            return;
        }

        ArrayList<WireData> data = wire.getWireData();
        int size = data.size();
        int[] thresholdData = new int[size];
        int[] marginTopData = new int[size];
        int[] marginDwnData = new int[size];
        int[] valueData = new int[size];
        int[] xPoints = new int[size];

        for (int i = 0; i < data.size(); i++) {
            WireData wd = data.get(i);
            valueData[i] = calculate(wd.getValue());
            thresholdData[i] = calculate(wd.getThreshold());
            marginTopData[i] = calculate(Math.max(wd.getThreshold() - wd.getMargin(), 0));
            marginDwnData[i] = calculate(Math.min(wd.getThreshold() + wd.getMargin(), 255));
            xPoints[i] = i;
        }

        Color old = g.getColor();
        g.setColor(marginColor);
        g.drawPolyline(xPoints, marginTopData, size);
        g.drawPolyline(xPoints, marginDwnData, size);
        g.setColor(thresholdColor);
        g.drawPolyline(xPoints, thresholdData, size);
        g.setColor(valueColor);
        g.drawPolyline(xPoints, valueData, size);
        g.setColor(old);
    }

    private int calculate(int value) {
        // At the top is 0,0 and the bottom is 0,getHeight()
        int result = getHeight() - (value * getHeight() / 256);
        if (result == getHeight()) {
            result--;
        }
        return result;
    }

    public void setData(Wire wire) {
        this.wire = wire;
    }

    public void highlight(int value) {
        highlightState = value;
        if ( value < 1 || value > 4 ) {
            highlightState = 0;
            setBackground(DEFAULT_BACKGROUND);
        }
        repaint();
    }

    private void drawHighlight(Graphics g) {
        if ( highlightState > 0 ) {
            Color old = g.getColor();
            g.setColor(HIGHLIGHT_BACKGROUND);
            int section = getHeight()/4; 
            int start = 1 + (int)((4-highlightState) * section);
            if ( start + section >= getHeight() ) {
                section = getHeight() - start - 1;
            }
            g.fillRect(1, start, getWidth()-2, section);
            g.setColor(old);
        }
    }
}
