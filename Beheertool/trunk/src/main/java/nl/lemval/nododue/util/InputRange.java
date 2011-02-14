/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package nl.lemval.nododue.util;

import org.apache.commons.lang.StringUtils;

/**
 *
 * @author Michael
 */
public class InputRange {

    private String name;
    private double start;
    private double end;
    private String postfix;
    private int decimals = 0;

    public InputRange(String name, double start, double end, double curve, String postfix, int dec) {
        this.name = name;
        this.start = start;
        this.end = end;
        this.postfix = postfix;
        this.decimals = dec;
    }

    public static InputRange fromString(String data) {
        try {
            String[] elements = data.split("\\|");
            int decimals = (elements.length > 5 ? Integer.parseInt(elements[5]) : 0);
            String postfix = (elements.length > 4 ? elements[4] : null);
            double curve = (elements.length > 3 ? Double.parseDouble(elements[3]) : 1);
            return new InputRange(
                    elements[0],
                    Double.parseDouble(elements[1]),
                    Double.parseDouble(elements[2]),
                    curve, postfix, decimals);
        } catch (Exception e) {
            return null;
        }
    }
    
    public String asString() {
        return name + "|" + start + "|" + end + "|" + 0 + "|" + postfix + "|" + decimals;
    }

    private double convertValue(int value) {
        double result = start + (end-start)/255*value;
        double factor = Math.pow(10, decimals);
        return Math.round(result*factor)/factor;
    }

    @Override
    public String toString() {
        return name;
    }

    public String getDescription(int value) {
        StringBuilder sb = new StringBuilder(name);
        sb.append(": ");
        if ( decimals == 0 ) {
            sb.append((int)convertValue(value));
        } else {
            sb.append(convertValue(value));
        }
        if ( StringUtils.isNotBlank(postfix) ) {
            sb.append(" ");
            sb.append(postfix);
        }
        return sb.toString();
    }

    public int getDecimals() {
        return decimals;
    }

    public double getEnd() {
        return end;
    }

    public String getName() {
        return name;
    }

    public String getPostfix() {
        return postfix;
    }

    public double getStart() {
        return start;
    }

    public void setDecimals(int decimals) {
        this.decimals = decimals;
    }

    public void setEnd(double end) {
        this.end = end;
    }

    public void setName(String name) {
        this.name = name;
    }

    public void setPostfix(String postfix) {
        this.postfix = postfix;
    }

    public void setStart(double start) {
        this.start = start;
    }
}
