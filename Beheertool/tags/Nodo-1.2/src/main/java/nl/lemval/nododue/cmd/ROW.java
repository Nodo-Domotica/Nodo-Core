/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package nl.lemval.nododue.cmd;

/**
 * Represents the columns in the command XLS structure.
 * 
 * @author Michael
 */
public enum ROW {

    Command(0, "Command"),
    Description(1, "Description"),
    Explanation(2, "Explanation"),
    ParName1(3, "Par1"),
    ParOpts1(4, "Par1opt"),
    ParName2(5, "Par2"),
    ParOpts2(6, "Par2opt"),
    FlagAction(7, "Action"),
    FlagSetting(8, "Setting"),
    FlagEvent(9, "Event"),
    FlagMultiNodo(10, "MultiNodo"),
    QueryRange(11, "QueryRange"),
    FlagSerialOnly(12, "SerialOnly"),
    FlagDevice(13, "Device");
    
    public final int val;
    public final String col;

    ROW(int value, String colName) {
        this.val = value;
        this.col = colName;
    }
}
