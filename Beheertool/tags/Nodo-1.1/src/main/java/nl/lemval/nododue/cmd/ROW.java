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
    Code(1, "Code"),
    Description(2, "Description"),
    Explanation(3, "Explanation"),
    ParName1(4, "Par1"),
    ParOpts1(5, "Par1opt"),
    ParName2(6, "Par2"),
    ParOpts2(7, "Par2opt"),
    FlagMacro(8, "Macro"),
    FlagAction(9, "Action"),
    FlagSetting(10, "Setting"),
    FlagEvent(11, "Event"),
    FlagMultiNodo(12, "MultiNodo"),
    QueryRange(13, "QueryRange"),
    FlagSerialOnly(14, "SerialOnly");
    
    public final int val;
    public final String col;

    ROW(int value, String colName) {
        this.val = value;
        this.col = colName;
    }
}
