/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package nl.lemval.nododue.component;

import java.awt.Color;
import java.awt.Graphics;
import javax.swing.JPanel;

/**
 *
 * @author Michael
 */
public class RawSignalGraphPanel extends JPanel {

    private int[] signal;
    private int[] points;
    private int[] values;
    private int total = 0;
    private static final Color valueColor = Color.BLUE;
    private static final Color inactiveColor = Color.LIGHT_GRAY;
    private static final Color hiddenColor = Color.BLACK;
    private int startPosition = 0;
    private int endPosition = -1;
    private boolean startState = true;
    private double divider = 0;

    public RawSignalGraphPanel() {
    }

    public void setStartState(boolean value) {
        if (this.startState != value) {
            this.startState = value;
            setSignal(signal);
        }
    }

    public void setSignal(int[] data) {
        signal = data.clone();
        if (signal.length == 0) {
            return;
        }
        points = new int[2 * signal.length + 1];
        values = new int[2 * signal.length + 1];
        total = 0;
        for (int s : signal) {
            total += s;
        }

        int high = 5;
        int low = getHeight() - 5;
        int location = 0;
        divider = (1.0 * total) / getWidth();
        boolean isOne = startState;
        int pointIndex = 0;
        int signalIndex = 0;

        // Set the first point to start with
        points[pointIndex] = location;
        values[pointIndex] = (isOne ? high : low);
        pointIndex++;

        do {
            isOne = !isOne;
            int s = signal[signalIndex++];

            location += (int) ((divider / 2 + s) / divider);
            points[pointIndex] = location;
            values[pointIndex] = (isOne ? low : high);
            pointIndex++;
            points[pointIndex] = location;
            values[pointIndex] = (isOne ? high : low);
            pointIndex++;
        } while (signalIndex < signal.length);
    }

    public int[] getActiveSignal() {
        if (divider == 0 || signal.length == 0) {
            // No signal yet
            return new int[0];
        }
        int start = 0;
        int end = signal.length;
        for (int i = 0; i < points.length; i++) {
            int point = points[i];
            if (startPosition > point) {
                start = i / 2;
            }
            if (endPosition < point) {
                end = i / 2 + 1;
                break;
            }
        }
        int length = end - start;
        int[] result = new int[length];
        System.arraycopy(signal, start, result, 0, length);
        return result;
    }

    @Override
    public void paint(Graphics g) {
        super.paint(g);

        Color old = g.getColor();

        if (total > 0) {
//	    int startIndex=0;
//	    int endIndex=values.length-1;
//	    for (int i = 0; i < points.length; i++) {
//		int point = points[i];
//		if ( point > startPosition ) {
//		    startIndex = point;
//		}
//		if ( point > endPosition ) {
//		    endIndex = point;
//		    break;
//		}
//	    }
//	    int[] segmentPoints = new int[points.length];
//	    int[] segmentValues = new int[values.length];
//	    System.arraycopy(points, 0, segmentPoints, 0, startIndex);
//	    System.arraycopy(values, 0, segmentValues, 0, startIndex);
//
//	    g.setColor(inactiveColor);
//	    g.drawPolyline(segmentPoints, segmentValues, startIndex);
//
//	    System.arraycopy(points, startIndex, segmentPoints, 0, points.length-startIndex);
//	    System.arraycopy(values, startIndex, segmentValues, 0, values.length-startIndex);

            g.setColor(valueColor);
            g.drawPolyline(points, values, values.length);
//	    System.out.print("Line drawn: ");
//	    for (int i = 0; i < points.length; i++) {
//		System.out.print("[" + points[i] + "," + values[i] + "] ");
//	    }
//	    System.out.println();
        }
        g.setColor(hiddenColor);
        g.drawLine(startPosition, 0, startPosition, getHeight());
        g.drawLine(endPosition, 0, endPosition, getHeight());
        g.setColor(old);

    }

    public void setBounds(int start, int end) {
        startPosition = start;
        endPosition = end;
    }

    public int[] getSignal() {
        return signal;
    }
}
