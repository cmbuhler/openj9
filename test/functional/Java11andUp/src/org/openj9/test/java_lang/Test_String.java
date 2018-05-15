/*******************************************************************************
 * Copyright (c) 2018, 2018 IBM Corp. and others
 *
 * This program and the accompanying materials are made available under
 * the terms of the Eclipse Public License 2.0 which accompanies this
 * distribution and is available at https://www.eclipse.org/legal/epl-2.0/
 * or the Apache License, Version 2.0 which accompanies this distribution and
 * is available at https://www.apache.org/licenses/LICENSE-2.0.
 *
 * This Source Code may also be made available under the following
 * Secondary Licenses when the conditions for such availability set
 * forth in the Eclipse Public License, v. 2.0 are satisfied: GNU
 * General Public License, version 2 with the GNU Classpath
 * Exception [1] and GNU General Public License, version 2 with the
 * OpenJDK Assembly Exception [2].
 *
 * [1] https://www.gnu.org/software/classpath/license.html
 * [2] http://openjdk.java.net/legal/assembly-exception.html
 *
 * SPDX-License-Identifier: EPL-2.0 OR Apache-2.0 OR GPL-2.0 WITH Classpath-exception-2.0 OR LicenseRef-GPL-2.0 WITH Assembly-exception
 *******************************************************************************/
package org.openj9.test.java_lang;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

import org.testng.Assert;
import org.testng.annotations.Test;
import org.testng.log4testng.Logger;

/**
 * This test Java.lang.String API added in Java 11 and later version.
 *
 */
public class Test_String {
	public static Logger logger = Logger.getLogger(Test_String.class);

	/*
	 * Test Java 11 API String.valueOfCodePoint(codePoint)
	 */
	@Test(groups = { "level.sanity" })
	public void testValueOfCodePoint() throws IllegalAccessException, IllegalArgumentException, InvocationTargetException, NoSuchMethodException, SecurityException {
		Method method = String.class.getDeclaredMethod("valueOfCodePoint", int.class);
		method.setAccessible(true);
		// Verify that for each valid Unicode code point i,
		// i equals to String.valueOfCodePoint(i).codePointAt(0).
		for (int i = Character.MIN_CODE_POINT; i <= Character.MAX_CODE_POINT; i++) {
			String str = (String) method.invoke(null, i);
			Assert.assertEquals(i, str.codePointAt(0), "Unicode code point mismatch at " + i);
		}
		// Verify that IllegalArgumentException is thrown for Character.MIN_CODE_POINT - 1.
		try {
			method.invoke(null, Character.MIN_CODE_POINT - 1);
			Assert.fail("Failed to detect invalid Unicode point - Character.MIN_CODE_POINT - 1");
		} catch (IllegalArgumentException | InvocationTargetException e) {
			// Expected exception
		}
		// Verify that IllegalArgumentException is thrown for Character.MAX_CODE_POINT + 1.
		try {
			method.invoke(null, Character.MAX_CODE_POINT + 1);
			Assert.fail("Failed to detect invalid Unicode point - Character.MAX_CODE_POINT + 1");
		} catch (IllegalArgumentException | InvocationTargetException e) {
			// Expected exception
		}
	}

	/*
	 * Test Java 11 method String.repeat(count)
	 */
	@Test(groups = { "level.sanity" })
	public void testRepeat() {
		String empty = "";
		String latin1 = "abc123";
		String nonLatin1 = "abc\u0153";
		String str;

		// Verify that correct strings are produced under normal conditions
		Assert.assertTrue(latin1.repeat(3).equals("abc123abc123abc123"), "Repeat failed to produce correct string (LATIN1)");
		Assert.assertTrue(nonLatin1.repeat(3).equals("abc\u0153abc\u0153abc\u0153"), "Repeat failed to produce correct string (non-LATIN1)");

		Assert.assertTrue(empty.repeat(0) == empty, "Failed to return identical empty string - empty base (0)");
		Assert.assertTrue(empty.repeat(2) == empty, "Failed to return identical empty string - empty base (2)");
		Assert.assertTrue(latin1.repeat(0) == empty, "Failed to return identical empty string - nonempty base (0)");

		// Verify that IllegalArgumentException is thrown for count < 0
		try {
			str = empty.repeat(-1);
			Assert.fail("Failed to detect invalid count -1");
		} catch (IllegalArgumentException e) {
			// Expected exception
		}

		try {
			str = latin1.repeat(-3);
			Assert.fail("Failed to detect invalid count -3");
		} catch (IllegalArgumentException e) {
			// Expected exception
		}

		// Verify that a reference to the instance that called the function is returned for count == 1
		Assert.assertTrue(latin1.repeat(1) == latin1, "Should be identical on count 1");
	}
}
