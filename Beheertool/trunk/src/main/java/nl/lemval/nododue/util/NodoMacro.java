/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package nl.lemval.nododue.util;


import nl.lemval.nododue.cmd.NodoCommand;

/**
 *
 * @author Michael
 */
public class NodoMacro {

    private NodoCommand event;
    private NodoCommand action;

    public NodoMacro(NodoCommand event, NodoCommand action) {
        this.event = event;
        this.action = action;
    }

    @Override
    public String toString() {
        StringBuilder builder = new StringBuilder();
        builder.append(event);
        builder.append(" ==> ");
        builder.append(action);
        return builder.toString();
    }

    @Override
    @SuppressWarnings("EqualsWhichDoesntCheckParameterClass")
    public boolean equals(Object obj) {
        return this.toString().equals(obj.toString());
    }

    @Override
    public int hashCode() {
        return toString().hashCode();
    }

    public NodoCommand getAction() {
        return action;
    }

    public NodoCommand getEvent() {
        return event;
    }
}
