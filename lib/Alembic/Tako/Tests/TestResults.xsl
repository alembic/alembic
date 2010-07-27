<?xml version="1.0" encoding="UTF-8" standalone="no" ?>

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="TestRun">

<html>

<h1>
Test Results
</h1>
<br />

<xsl:apply-templates select="Statistics"/>
<xsl:apply-templates select="FailedTests"/>
<xsl:apply-templates select="SuccessfulTests"/>

</html>

</xsl:template>



<xsl:template match="FailedTests">

<h2>
Failed Tests
</h2>
<br />

<table border="1">
<tr> <td>Test Name</td> <td>Failure Type</td> <td>Message</td> <td>Where</td> </tr>
<xsl:apply-templates select="FailedTest" />
</table>

</xsl:template>



<xsl:template match="FailedTest">

<tr>
<td><xsl:value-of select="Name" /></td>
<td><xsl:value-of select="FailureType" /></td>
<td><pre><xsl:value-of select="Message" /></pre></td>
<xsl:apply-templates select="Location" />
</tr>

</xsl:template>



<xsl:template match="Location">

<td>Line <xsl:value-of select="Line" /> of <xsl:value-of select="File" /></td>

</xsl:template>



<xsl:template match="SuccessfulTests">

<h2>
Passed Tests
</h2>
<br />

<xsl:apply-templates select="Test" />

</xsl:template>



<xsl:template match="Test">

<xsl:value-of select="Name" />
<br/>

</xsl:template>



<xsl:template match="Statistics">

<h2>
Summary
</h2>
<br />

<table border="1">
<tr><td>Number Of Tests</td> <td><xsl:value-of select="Tests" /></td> </tr>
<tr><td>Number Of Errors</td> <td><xsl:value-of select="Errors" /></td> </tr>
<tr><td>Number Of Test Failures</td> <td><xsl:value-of select="Failures" /></td> </tr>
<tr><td>Total Failures</td> <td><xsl:value-of select="FailuresTotal" /></td> </tr>
</table>

</xsl:template>

</xsl:stylesheet>
