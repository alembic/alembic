{

  protected:
    BoolPropertyPtr m_orthographic;
    DoublePropertyPtr m_centerOfInterest;
    // And so on
    

}

void CameraWriter::write( float iFrame, const CameraData &iCam )
{
    WriteSampleSelector wss = this->takoFrameToSampleSelector( iFrame );
    
    m_orthographic->setAndWrite( iCam.orthographic, wss );

};

MetaData
{
    m_metaData["bob"] = aThing;
    std::string "protocol";
    DataType;
    std::string "name";

    "bob=int(7);jane=string(hello);lisa=float(47.2)";

    
    // aThing can be int, double, float, etc... (pod)
}
