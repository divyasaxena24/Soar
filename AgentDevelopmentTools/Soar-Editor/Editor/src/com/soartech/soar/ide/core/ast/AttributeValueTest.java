/*
 *Copyright (c) 2009, Soar Technology, Inc.
 *All rights reserved.
 *
 *Redistribution and use in source and binary forms, with or without modification,   *are permitted provided that the following conditions are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name of Soar Technology, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY  *EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED   *WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.   *IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,   *INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT   *NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR   *PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,    *WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)   *ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE    *POSSIBILITY OF SUCH *DAMAGE. 
 *
 * 
 */
package com.soartech.soar.ide.core.ast;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.List;

public final class AttributeValueTest implements Serializable
{

	private static final long serialVersionUID = -4314738660614931675L;

	// Data Members
    boolean negated = false;

    List<AttributeTest> attributeTests = new ArrayList<AttributeTest>();

    List<ValueTest> valueTests = new ArrayList<ValueTest>();

    // Accessors
    public final void negate()
    {
        negated = true;
    }

    public final void add(AttributeTest at)
    {
        attributeTests.add(at);
    }

    public final void add(ValueTest vt)
    {
        valueTests.add(vt);
    }

    public final List<AttributeTest> getAttributeTests()
    {
        return attributeTests;
    }

    public final List<ValueTest> getValueTests()
    {
        return valueTests;
    }

    public final boolean isNegated()
    {
        return negated;
    }

    /*
     * (non-Javadoc)
     * 
     * @see java.lang.Object#toString()
     */
    @Override
    public String toString()
    {
        String r = attributeTests + ", " + valueTests;
        if (isNegated())
        {
            r = "-{" + r + "}";
        }
        return r;
    }

}