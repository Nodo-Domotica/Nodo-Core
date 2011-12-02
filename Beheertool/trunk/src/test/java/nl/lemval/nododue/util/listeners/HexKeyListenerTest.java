/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package nl.lemval.nododue.util.listeners;

import java.awt.event.KeyEvent;
import javax.swing.JTextField;
import junit.framework.TestCase;

/**
 *
 * @author Michael
 */
public class HexKeyListenerTest extends TestCase {

    private JTextField tf = new JTextField();
    private HexKeyListener twoFieldListener = new HexKeyListener(2);
    private static final char NULL = (char) 0;

    public HexKeyListenerTest(String testName) {
        super(testName);
    }

    @Override
    protected void setUp() throws Exception {
        super.setUp();
    }

    @Override
    protected void tearDown() throws Exception {
        super.tearDown();
    }

    /**
     * Test of checkHexCode method, of class HexKeyListener.
     */
    public void testCheckHexCode() {
        test("0", 1, KeyEvent.VK_0, '0', '0');
        test("0", 1, KeyEvent.VK_3, '3', '3');
        test("0", 1, KeyEvent.VK_X, 'x', 'x');
        test("0", 1, KeyEvent.VK_X, 'X', 'x');
        test("FF", 1, KeyEvent.VK_F, 'f', NULL);
    }

    public void testCheckHexCode2() {
        test("0x", 1, KeyEvent.VK_0, '0', NULL);
        test("0x", 2, KeyEvent.VK_0, '0', '0');
        test("0x", 2, KeyEvent.VK_3, '3', '3');
        test("0x", 2, KeyEvent.VK_X, 'x', NULL);
        test("0x", 2, KeyEvent.VK_X, 'X', NULL);
        test("0xF", 3, KeyEvent.VK_F, 'f', 'F');
    }


    private void test(String startText, int idx, int code, char c, char expected) {
        tf.setText(startText);
        tf.setCaretPosition(idx);
        KeyEvent evt = new KeyEvent(tf, 0, 0, 0, code, c);
        twoFieldListener.checkHexCode(evt);

        if ( evt.isConsumed() )
            assertEquals(expected, NULL);
        else {
            System.out.println(evt.getKeyChar() + ":" + evt.getKeyCode());
            assertEquals(expected, evt.getKeyChar());
        }
    }

    /**
     * Test of keyTyped method, of class HexKeyListener.
     */
    public void testKeyTyped() {
        KeyEvent evt = new KeyEvent(tf, 0,0,0,0,(char)0);
        new HexKeyListener().keyTyped(evt);
    }

    /**
     * Test of keyPressed method, of class HexKeyListener.
     */
    public void testKeyPressed() {
        new HexKeyListener().keyPressed(null);
    }

    /**
     * Test of keyReleased method, of class HexKeyListener.
     */
    public void testKeyReleased() {
        new HexKeyListener().keyReleased(null);
    }

}
