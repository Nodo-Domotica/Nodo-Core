/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package nl.lemval.nododue.util;

import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;

/**
 *
 * @author Michael
 */
public class HistoryElem {

    private String elem;
    private Date time;
    private boolean request;

    private static final DateFormat format = new SimpleDateFormat("HH:mm:ss");

    public HistoryElem(String elem, boolean isRequest) {
        this.elem = elem;
        this.request = isRequest;
        this.time = new Date();
    }

    @Override
    public String toString() {
        StringBuilder builder = new StringBuilder();
        builder.append(format.format(time));
        builder.append(" [");
        builder.append(request ? ">" : "<");
        builder.append("] ");
        builder.append(elem);
        return builder.toString();
    }
}
