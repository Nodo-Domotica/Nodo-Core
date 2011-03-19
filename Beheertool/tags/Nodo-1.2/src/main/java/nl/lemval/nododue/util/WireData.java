/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package nl.lemval.nododue.util;

/**
 *
 * @author Michael
 */
public class WireData {

    private int value = -1;
    private int margin = -1;
    private int threshold = -1;
    private int range = -1;
    
    private boolean state;

    public WireData() {
    }

    public int getMargin() {
        return margin;
    }

    public int getValue() {
        return value;
    }

    public int getThreshold() {
        return threshold;
    }

    public int getRange() {
        return range;
    }

    public void setRange(int range) {
        if (this.range == -1) this.range = range;
    }

    public void setMargin(int margin) {
        if (this.margin == -1) this.margin = margin;
    }

    public void setThreshold(int threshold) {
        if ( this.threshold == -1 ) this.threshold = threshold;
    }

    public void setValue(int value) {
        if ( this.value == -1 ) this.value = value;
    }

    public void setState(boolean value) {
        state = value;
    }

    public boolean isState() {
        return state;
    }

    @Override
    public String toString() {
        return "(" + state + ", " + value + ", " + threshold + ", " + margin + ")";
    }
}
