<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0" xmlns:fo="http://www.w3.org/1999/XSL/Format" xmlns:fox="http://xml.apache.org/fop/extensions" xmlns:xi="http://www.w3.org/2001/Xinclude" xmlns:foa="http://fabio">
  <xsl:output method="xml" encoding="UTF-8" indent="yes"></xsl:output>
  <xsl:template match="/">
    <fo:root xmlns:fo="http://www.w3.org/1999/XSL/Format">
      <fo:layout-master-set>
        <fo:simple-page-master margin-left="1.4in" margin-right="1.15in" page-width="8.5in" margin-bottom="1.25in" master-name="StandardPage" margin-top="1.25in" page-height="11in">
          <fo:region-body ></fo:region-body >
          <fo:region-before></fo:region-before>
        </fo:simple-page-master>
        <fo:page-sequence-master master-name="DocumentBody">
          <fo:repeatable-page-master-reference master-reference="StandardPage"/>
        </fo:page-sequence-master>
      </fo:layout-master-set>
      <fo:page-sequence  master-reference="DocumentBody">
        <fo:flow  flow-name="xsl-region-body">
          <xsl:apply-templates select="document('Thesis Proposal.xml')/document/section"/>
        </fo:flow >
      </fo:page-sequence >
    </fo:root>
  </xsl:template>
</xsl:stylesheet>
