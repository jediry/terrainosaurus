<?xml version="1.0" encoding="UTF-8"?>
<!-- vim: set encoding=utf8: -->

<xsl:stylesheet
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0"
  xmlns:java="http://xml.apache.org/xalan/java"
  xmlns:svg="http://www.w3.org/2000/svg"
  xmlns:sodipodi="http://inkscape.sourceforge.net/DTD/sodipodi-0.dtd"
  xmlns:inkscape="http://www.inkscape.org/namespaces/inkscape"
  exclude-result-prefixes="java">
  
  <xsl:output method="xml" indent="yes"/>

  <!-- Copy elements and attributes, by default -->
  <xsl:template match="*">
    <xsl:copy>
      <xsl:for-each select="@*">
        <xsl:apply-templates select="."/>
      </xsl:for-each>
      <xsl:apply-templates/>
    </xsl:copy>
  </xsl:template>
  <xsl:template match="@*">
    <xsl:copy/>
  </xsl:template>

  <!-- Alter just a few of the attributes -->
  <xsl:template match="@pagecolor">
    <xsl:attribute name="pagecolor">#074488</xsl:attribute>
  </xsl:template>
  <xsl:template match="@inkscape:pageopacity">
    <xsl:attribute name="inkscape:pageopacity">1.0</xsl:attribute>
  </xsl:template>
  <xsl:template match="@style">
    <xsl:attribute name="style">
      <xsl:variable name="oldString" select="string(.)"/>
      <xsl:value-of select="java:replaceAll($oldString, string('#000000'), string('#FFFFFF'))"/>
    </xsl:attribute>
  </xsl:template>
  <xsl:template match="svg:marker/svg:path/@style">
    <xsl:attribute name="style">
      <xsl:value-of select="concat(., ';fill:#FFFFFF')"/>
    </xsl:attribute>
  </xsl:template>
  

  <!--xsl:template match="svg:marker/svg:path">
    <xsl:copy>
      <xsl:for-each select="@*">
        <xsl:choose>
          <!- We have to change this... ->
          <xsl:when test="name() = 'style'">
            <xsl:attribute name="style">
              <xsl:value-of select="concat(., ';fill:#FFFFFF')"/>
            </xsl:attribute>
          </xsl:when>
          <!- We just copy these... ->
          <xsl:otherwise>
            <xsl:copy/>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:for-each>

      <xsl:apply-templates/>
    </xsl:copy>
  </xsl:template-->

  <!-- Copy elements, altering the style attribute -->
  <!--xsl:template match="*">
    <xsl:copy>
      <xsl:for-each select="@*">
        <xsl:choose>
          <!- We have to change this... ->
          <xsl:when test="name() = 'style'">
            <xsl:attribute name="style">
              <xsl:variable name="oldString" select="string(.)"/>
              <xsl:value-of select="java:replaceAll($oldString, 
                string('#000000'), string('#FFFFFF'))"/>
            </xsl:attribute>
          </xsl:when>
          <- We just copy these... ->
          <xsl:otherwise>
            <xsl:copy/>
          </xsl:otherwise>
        </xsl:choose>
      </xsl:for-each>

      <xsl:apply-templates/>
    </xsl:copy>
  </xsl:template-->

</xsl:stylesheet>
