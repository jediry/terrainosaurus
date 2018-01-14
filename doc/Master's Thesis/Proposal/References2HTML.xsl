<?xml version="1.0"?> 

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
  <xsl:output method="html" omit-xml-declaration="yes" indent="yes"/>

  <!-- Top-level rule: sorts references by author's last name -->
  <xsl:template match="/references">
    <xsl:for-each select="reference">
      <xsl:sort select="author[1]/last-name"/>
      <div style="padding-bottom: 6pt; padding-left: 20px; text-indent: -20px;">
        <xsl:apply-templates select="."/>
      </div>
    </xsl:for-each>
  </xsl:template>


  <!-- Formatting of author lists -->
  <xsl:template name="format-authors">
    <xsl:param name="author-list"/>
    <xsl:param name="full-name"/>
    <xsl:choose>
      <xsl:when test="count($author-list) = 0">
        Anonymous Authors
      </xsl:when>
      <xsl:when test="count($author-list) = 1">
        <xsl:apply-templates select="$author-list[1]">
          <xsl:with-param name="full-name" select="$full-name"/>
        </xsl:apply-templates>
      </xsl:when>
      <xsl:when test="count($author-list) = 2">
        <xsl:apply-templates select="$author-list[1]">
          <xsl:with-param name="full-name" select="$full-name"/>
        </xsl:apply-templates> and
        <xsl:apply-templates select="$author-list[2]">
          <xsl:with-param name="full-name" select="$full-name"/>
        </xsl:apply-templates>
      </xsl:when>
      <xsl:otherwise>
        <xsl:apply-templates select="$author-list[1]">
          <xsl:with-param name="full-name" select="$full-name"/>
        </xsl:apply-templates> et al.
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <!-- Formatting an individual author's name (gotta be careful about whitespace) -->
  <xsl:template match="author">
    <xsl:param name="full-name"/>
    <xsl:value-of select="normalize-space(last-name)"/><xsl:if test="first-name and $full-name">, <xsl:value-of 
    select="normalize-space(first-name)"/></xsl:if><xsl:if test="middle-name and $full-name"> <xsl:value-of select="concat(' ', 
    normalize-space(middle-name))"/></xsl:if></xsl:template>


  <!-- Software -->
  <xsl:template match="reference[@type = 'software']" priority="1">
    <i><xsl:value-of select="normalize-space(title)"/></i>.
    <xsl:if test="developer/name">
      <xsl:value-of select="normalize-space(developer/name)"/> <i> (dev)</i>.
    </xsl:if>
    <xsl:if test="publisher/name">
      <xsl:value-of select="normalize-space(publisher/name)"/> <i> (pub)</i>.
    </xsl:if>
    <xsl:if test="date/year">
      <xsl:value-of select="normalize-space(date/year)"/>.
    </xsl:if>
    <xsl:if test="url">
      <xsl:element name="a">
        <xsl:attribute name="href"><xsl:value-of select="normalize-space(url)"/></xsl:attribute>
        <xsl:value-of select="normalize-space(url)"/></xsl:element>.
    </xsl:if>
  </xsl:template>


  <!-- Websites -->
  <xsl:template match="reference[@type = 'website']" priority="1">
    <xsl:call-template name="format-authors">
      <xsl:with-param name="author-list" select="author"/>
      <xsl:with-param name="full-name" select="true()"/>
    </xsl:call-template>.
    <i><xsl:value-of select="normalize-space(title)"/></i>.
    <xsl:element name="a">
      <xsl:attribute name="href"><xsl:value-of select="normalize-space(url)"/></xsl:attribute>
      <xsl:value-of select="normalize-space(url)"/></xsl:element>.
    <xsl:if test="publisher/name">
      <xsl:value-of select="normalize-space(publisher/name)"/><xsl:if test="date/year">, <xsl:value-of 
      select="normalize-space(date/year)"/></xsl:if>.
    </xsl:if>
  </xsl:template>


  <!-- Conference proceedings -->
  <xsl:template match="reference[@type = 'proceedings' or @type = 'course-notes']" priority="1">
    <xsl:call-template name="format-authors">
      <xsl:with-param name="author-list" select="author"/>
      <xsl:with-param name="full-name" select="true()"/>
    </xsl:call-template>.
    <i><xsl:value-of select="normalize-space(title)"/></i>.
    In proceedings of <b><xsl:value-of select="normalize-space(conference)"/></b>.
    <xsl:value-of select="normalize-space(publisher/name)"/>, <xsl:value-of select="normalize-space(date/year)"/>.
  </xsl:template>


  <!-- Other -->
  <xsl:template match="reference" priority="0">
    <xsl:call-template name="format-authors">
      <xsl:with-param name="author-list" select="author"/>
      <xsl:with-param name="full-name" select="true()"/>
    </xsl:call-template>.
    <i><xsl:value-of select="normalize-space(title)"/></i>.
    <xsl:if test="publisher/name">
      <xsl:value-of select="publisher/name"/>,
    </xsl:if>
    <xsl:value-of select="date/year"/>.
  </xsl:template>

</xsl:stylesheet>
