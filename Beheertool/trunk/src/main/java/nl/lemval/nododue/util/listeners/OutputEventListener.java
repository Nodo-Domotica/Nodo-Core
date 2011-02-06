/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package nl.lemval.nododue.util.listeners;

/**
 *
 * @author Michael
 */
public interface OutputEventListener {
    public void handleOutputLine(String message);
    public void handleClear();
}
