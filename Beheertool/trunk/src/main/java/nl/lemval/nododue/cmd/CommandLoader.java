/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package nl.lemval.nododue.cmd;

import java.io.InputStream;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashSet;
import java.util.TreeMap;
import jxl.Sheet;
import jxl.Workbook;
import nl.lemval.nododue.NodoDueManager;

/**
 *
 * @author Michael
 */
public class CommandLoader {

    private static TreeMap<String,CommandInfo> commands = new TreeMap<String,CommandInfo>();

    public static Collection<CommandInfo> getActions(CommandType type) {
        if ( commands.isEmpty()) {
            initCommands();
        }
        Collection<CommandInfo> result = new ArrayList<CommandInfo>();
        for (CommandInfo ncd : commands.values()) {
            if ( ncd.getType().is(type) ) {
                result.add(ncd);
            }
        }
        return result;
    }

    public static CommandInfo get(CommandInfo.Name name) {
	return get(name.name());
    }

    public static CommandInfo get(String name) {
        for (CommandInfo commandInfo : commands.values()) {
            if ( commandInfo.getName().equals(name) ) {
                return commandInfo;
            }
        }
        return null;
    }

    private static void initCommands() {
        InputStream stream = null;
        try {
            stream = NodoCommand.class
                .getResourceAsStream("/nl/lemval/nododue/resources/NodoCommandList.xls");
            Workbook workbook = Workbook.getWorkbook(stream);
            Sheet sheet = workbook.getSheet("NodoDue");

            int[] rows = findRows(sheet);
            int max = sheet.getRows();
            // First one is title row
            for (int i = 1; i < max; i++) {
                String name = sheet.getCell(rows[ROW.Command.val], i).getContents();
                if ( name != null && name.trim().length() > 0 ) {
                    String desc = sheet.getCell(rows[ROW.Description.val], i).getContents();
                    String expl = sheet.getCell(rows[ROW.Explanation.val], i).getContents();

                    CommandInfo action = new CommandInfo(name, desc, expl);
                    action.addParameter(
                            sheet.getCell(rows[ROW.ParName1.val], i).getContents(),
                            sheet.getCell(rows[ROW.ParOpts1.val], i).getContents());
                    action.addParameter(
                            sheet.getCell(rows[ROW.ParName2.val], i).getContents(),
                            sheet.getCell(rows[ROW.ParOpts2.val], i).getContents());
                    action.setQueryRange(
                            sheet.getCell(rows[ROW.QueryRange.val], i).getContents());
                    
                    CommandType type = action.getType();
                    if ( "Y".equals(sheet.getCell(rows[ROW.FlagAction.val], i).getContents()) ) {
                        type = type.add(CommandType.COMMAND);
                    }
                    if ( "Y".equals(sheet.getCell(rows[ROW.FlagEvent.val], i).getContents()) ) {
                        type = type.add(CommandType.EVENT);
                    }
                    if ( "Y".equals(sheet.getCell(rows[ROW.FlagDevice.val], i).getContents()) ) {
                        type = type.add(CommandType.DEVICE);
                    }
                    if ( "Y".equals(sheet.getCell(rows[ROW.FlagSetting.val], i).getContents()) ) {
                        type = type.add(CommandType.SETTING);
                    }
                    if ( "Y".equals(sheet.getCell(rows[ROW.FlagMultiNodo.val], i).getContents()) ) {
                        type = type.add(CommandType.MULTI);
                    }
                    if ( "Y".equals(sheet.getCell(rows[ROW.FlagSerialOnly.val], i).getContents()) ) {
                        type = type.add(CommandType.SERIALONLY);
                    }
                    action.setType(type);
                    commands.put(action.toString()+":"+type.toString(), action);
                }
            }
            workbook.close();
        } catch (Exception e) {
            NodoDueManager.showDialog("Commandloader.load_failed", e.getMessage(), e);
            try { if (stream != null) stream.close(); } catch (Exception ex) {}
        }
//        for (CommandInfo commandInfo : commands.values()) {
//            System.out.println("Added command " + commandInfo.getName() +
//                    ":" + commandInfo.getType().toString() );
//
//        }
    }

    private static int[] findRows(Sheet sheet) {
        int[] result = new int[ROW.values().length];

        for (int i = 0; i < sheet.getColumns(); i++) {
            String name = sheet.getCell(i, 0).getContents();
            for (int j = 0; j < ROW.values().length; j++) {
                ROW row = ROW.values()[j];
                if ( name.equals(row.col) ) {
                    result[row.val] = i;
                    break;
                }
            }
        }
        return result;
    }

    public static void removeActions(Collection<CommandInfo> cmds, CommandType type) {
        HashSet<CommandInfo> remove = new HashSet<CommandInfo>();
        for (CommandInfo commandInfo : cmds) {
            CommandType ct = commandInfo.getType();
            if ( ct.is(type) )
                remove.add(commandInfo);
        }
        for (CommandInfo commandInfo : remove) {
            cmds.remove(commandInfo);
        }
    }
}
