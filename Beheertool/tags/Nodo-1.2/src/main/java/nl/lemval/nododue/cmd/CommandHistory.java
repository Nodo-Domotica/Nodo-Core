/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package nl.lemval.nododue.cmd;

import java.util.ArrayList;
import nl.lemval.nododue.Options;

import nl.lemval.nododue.util.HistoryElem;

/**
 *
 * @author Michael
 */
public class CommandHistory {

    private ArrayList<HistoryElem> history;
    private static final Object lock = new Object();

    public CommandHistory() {
        history = new ArrayList<HistoryElem>();
    }

    public void addRequest(String cmd) {
        synchronized (lock) {
            history.add(new HistoryElem(cmd, true));
        }
        checkTruncate();
    }
    public void addResponse(String cmd) {
        Options.getInstance().registerResponse(cmd);
        synchronized (lock) {
            history.add(new HistoryElem(cmd, false));
        }
        checkTruncate();
    }

    public ArrayList<HistoryElem> getCommands() {
        ArrayList<HistoryElem> copy = new ArrayList<HistoryElem>();
        synchronized (lock) {
            copy.addAll(history);
        }
        return copy;
    }

    public void removeCommand(HistoryElem historyElem) {
        synchronized (lock) {
            history.remove(historyElem);
        }
    }

    private void checkTruncate() {
        int maxSize = Options.getInstance().getMaxHistorySize();
        if ( history.size() > maxSize ) {
            synchronized (lock) {
                int delta = maxSize/100;
                int newStart = history.size() - maxSize + (delta==0?1:delta);
                if ( newStart > 0 ) {
                    history.subList(0, newStart).clear();
                }
            }
        }
    }
}
