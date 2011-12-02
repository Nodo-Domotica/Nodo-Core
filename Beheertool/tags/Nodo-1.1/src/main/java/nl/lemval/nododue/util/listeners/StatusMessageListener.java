/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package nl.lemval.nododue.util.listeners;

/**
 *
 * @author Michael
 */
public interface StatusMessageListener {
    void showStatusMessage(String message);
    void readyForConnection(boolean state);
    void connected(boolean valid);
    void disconnected();
}
