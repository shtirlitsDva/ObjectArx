// (C) Copyright 2002-2007 by Autodesk, Inc. 
//
// Permission to use, copy, modify, and distribute this software in
// object code form for any purpose and without fee is hereby granted, 
// provided that the above copyright notice appears in all copies and 
// that both that copyright notice and the limited warranty and
// restricted rights notice below appear in all supporting 
// documentation.
//
// AUTODESK PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS. 
// AUTODESK SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF
// MERCHANTABILITY OR FITNESS FOR A PARTICULAR USE.  AUTODESK, INC. 
// DOES NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE
// UNINTERRUPTED OR ERROR FREE.
//
// Use, duplication, or disclosure by the U.S. Government is subject to 
// restrictions set forth in FAR 52.227-19 (Commercial Computer
// Software - Restricted Rights) and DFAR 252.227-7013(c)(1)(ii)
// (Rights in Technical Data and Computer Software), as applicable.
//

//-----------------------------------------------------------------------------
//----- AuPolyline.cpp : Implementation of AuPolyline
//-----------------------------------------------------------------------------
#include "StdAfx.h"
#include "AuPolyline.h"

//-----------------------------------------------------------------------------
Adesk::UInt32 AuPolyline::kCurrentVersionNumber = 1;

//-----------------------------------------------------------------------------
ACRX_DXF_DEFINE_MEMBERS(
	AuPolyline, AcDbPolyline,
	AcDb::kDHL_CURRENT, AcDb::kMReleaseCurrent,
	AcDbProxyEntity::kNoOperation, AUPOLYLINE,
	AUPOLYLINEDBXAPP
	| Product Desc : A description for your object
	| Company : Your company name
	| WEB Address : Your company WEB site address
)

//-----------------------------------------------------------------------------
AuPolyline::AuPolyline() : AcDbPolyline()
{}

AuPolyline::~AuPolyline()
{}

//-----------------------------------------------------------------------------
//----- AcDbObject protocols
//- Dwg Filing protocol
Acad::ErrorStatus AuPolyline::dwgOutFields(AcDbDwgFiler * pFiler) const
{
	assertReadEnabled();
	//----- Save parent class information first.
	Acad::ErrorStatus es = AcDbPolyline::dwgOutFields(pFiler);
	if (es != Acad::eOk)
		return (es);
	//----- Object version number needs to be saved first
	if ((es = pFiler->writeUInt32(AuPolyline::kCurrentVersionNumber)) != Acad::eOk)
		return (es);
	//----- Output params
	//.....

	return (pFiler->filerStatus());
}

Acad::ErrorStatus AuPolyline::dwgInFields(AcDbDwgFiler * pFiler)
{
	assertWriteEnabled();
	//----- Read parent class information first.
	Acad::ErrorStatus es = AcDbPolyline::dwgInFields(pFiler);
	if (es != Acad::eOk)
		return (es);
	//----- Object version number needs to be read first
	Adesk::UInt32 version = 0;
	if ((es = pFiler->readUInt32(&version)) != Acad::eOk)
		return (es);
	if (version > AuPolyline::kCurrentVersionNumber)
		return (Acad::eMakeMeProxy);
	//- Uncomment the 2 following lines if your current object implementation cannot
	//- support previous version of that object.
	//if ( version < AuPolyline::kCurrentVersionNumber )
	//	return (Acad::eMakeMeProxy) ;
	//----- Read params
	//.....

	return (pFiler->filerStatus());
}

//-----------------------------------------------------------------------------
//----- AcDbEntity protocols
Adesk::Boolean AuPolyline::subWorldDraw(AcGiWorldDraw * mode)
{
	assertReadEnabled();
	//Call base class first
	AcDbPolyline::subWorldDraw(mode);
	double szRef = 5.0;

	//===================================================================
	//DIRECTION AND VERTEX NUMBERING
	int signal = 1;
	double ht2 = szRef / 4.0;

	for (uint i = 0; i < numVerts(); i++)
	{
		AcGePoint3d pti;
		this->getPointAt(i, pti);

		//Draw vertex text
		CString strNum;
		strNum.Format(_T("%d"), i);
		AcGePoint3d ptTxt = pti + (AcGeVector3d::kXAxis * ht2) + (AcGeVector3d::kYAxis * ht2);
		mode->subEntityTraits().setColor(256); //ByLayer
		mode->geometry().text(ptTxt, AcGeVector3d::kZAxis, AcGeVector3d::kXAxis, ht2, 1.0, 0.0, strNum);

		//Arrow direction
		AcGePoint3d ptj;
		this->getPointAt(i < (numVerts() - 1) ? (i + 1) : 0, ptj);
		AcGeVector3d dir = (ptj - pti).normalize();

		//Side perpendicular vectors
		AcGeVector3d perp = dir;
		perp.rotateBy(3.141592 / 2.0, AcGeVector3d::kZAxis);
		AcGePoint3d pt1 = ptj - (dir * ht2) + (perp * (ht2 / 4.0));
		AcGePoint3d pt2 = ptj - (dir * ht2) - (perp * (ht2 / 4.0));
		AcGePoint3d pts[3];
		pts[0] = ptj;
		pts[1] = pt1;
		pts[2] = pt2;

		//Draw arrow polygon
		mode->subEntityTraits().setFillType(kAcGiFillAlways);
		mode->subEntityTraits().setColor(1);//Red
		mode->geometry().polygon(3, pts);
		mode->subEntityTraits().setFillType(kAcGiFillNever);
	}

	//------ Returning Adesk::kFalse here will force viewportDraw() call
	return (Adesk::kTrue);
}

void AuPolyline::subViewportDraw(AcGiViewportDraw * mode)
{
	assertReadEnabled();
	AcDbPolyline::subViewportDraw(mode);
}

Adesk::UInt32 AuPolyline::subViewportDrawLogicalFlags(AcGiViewportDraw * vd)
{
	assertReadEnabled();
	return (AcDbPolyline::subViewportDrawLogicalFlags(vd));
}

Adesk::UInt32 AuPolyline::subSetAttributes(AcGiDrawableTraits * traits)
{
	assertReadEnabled();
	return (AcDbPolyline::subSetAttributes(traits));
}

//- Osnap points protocol
Acad::ErrorStatus AuPolyline::subGetOsnapPoints(
	AcDb::OsnapMode osnapMode,
	Adesk::GsMarker gsSelectionMark,
	const AcGePoint3d & pickPoint,
	const AcGePoint3d & lastPoint,
	const AcGeMatrix3d & viewXform,
	AcGePoint3dArray & snapPoints,
	AcDbIntArray & geomIds) const
{
	assertReadEnabled();
	return (AcDbPolyline::subGetOsnapPoints(osnapMode, gsSelectionMark, pickPoint, lastPoint, viewXform, snapPoints, geomIds));
}

Acad::ErrorStatus AuPolyline::subGetOsnapPoints(
	AcDb::OsnapMode osnapMode,
	Adesk::GsMarker gsSelectionMark,
	const AcGePoint3d & pickPoint,
	const AcGePoint3d & lastPoint,
	const AcGeMatrix3d & viewXform,
	AcGePoint3dArray & snapPoints,
	AcDbIntArray & geomIds,
	const AcGeMatrix3d & insertionMat) const
{
	assertReadEnabled();
	return (AcDbPolyline::subGetOsnapPoints(osnapMode, gsSelectionMark, pickPoint, lastPoint, viewXform, snapPoints, geomIds, insertionMat));
}

//- Grip points protocol
Acad::ErrorStatus AuPolyline::subGetGripPoints(
	AcGePoint3dArray & gripPoints, AcDbIntArray & osnapModes, AcDbIntArray & geomIds
) const
{
	assertReadEnabled();
	////Acad::ErrorStatus es;
	//
	//gripPoints.removeAt(gripPoints.length() - 1);
	//AcGePoint3d center = GetPolylineCenter();
	//gripPoints.append(center);

	//----- This method is never called unless you return eNotImplemented 
	//----- from the new getGripPoints() method below (which is the default implementation)
	//return Acad::eOk;

	return (AcDbPolyline::subGetGripPoints(gripPoints, osnapModes, geomIds));
}

Acad::ErrorStatus AuPolyline::subMoveGripPointsAt(const AcDbIntArray & indices, const AcGeVector3d & offset)
{
	assertWriteEnabled();
	//----- This method is never called unless you return eNotImplemented 
	//----- from the new moveGripPointsAt() method below (which is the default implementation)

	if (indices[0] == 9)
	{
		this->transformBy(offset);
		return Acad::eOk;
	}
	else
	return (AcDbPolyline::subMoveGripPointsAt(indices, offset));
}

Acad::ErrorStatus AuPolyline::subGetGripPoints(
	AcDbGripDataPtrArray & grips, const double curViewUnitSize, const int gripSize,
	const AcGeVector3d & curViewDir, const int bitflags
) const
{
	assertReadEnabled();
	AcDbGripData* gpd = new AcDbGripData();
	gpd->setAppData((void*)9999); // Center grip code
	gpd->setGripPoint(GetPolylineCenter());
	grips.append(gpd);
	grips.removeAt(grips.length() - 1);
	AcDbPolyline::subGetGripPoints(grips, curViewUnitSize, gripSize, curViewDir, bitflags);
	return (Acad::eOk);
	//----- If you return eNotImplemented here, that will force AutoCAD to call
	//----- the older getGripPoints() implementation. The call below may return
	//----- eNotImplemented depending of your base class.
	//return (AcDbPolyline::subGetGripPoints(grips, curViewUnitSize, gripSize, curViewDir, bitflags));
}

Acad::ErrorStatus AuPolyline::subMoveGripPointsAt(
	const AcDbVoidPtrArray & gripAppData, const AcGeVector3d & offset,
	const int bitflags
)
{
	assertWriteEnabled();
	
	//for (int g = 0; g < gripAppData.length(); g++)
	//{
	//	//Get grip data back and see if it is our 0 Grip
	//	int i = (int)gripAppData.at(g);
	//	//If it is our grip, move the entire entity. If not, forward the call
	//	if (i == 9999)
	//	{
	//		this->transformBy(offset);
	//	}
	//	else
	//		return (AcDbCurve::subMoveGripPointsAt(gripAppData, offset, bitflags));
	//}
	//return (Acad::eOk);

	//----- If you return eNotImplemented here, that will force AutoCAD to call
	//----- the older getGripPoints() implementation. The call below may return
	//----- eNotImplemented depending of your base class.
	return (AcDbPolyline::subMoveGripPointsAt(gripAppData, offset, bitflags));
}

AcGePoint3d AuPolyline::GetPolylineCenter() const
{
	assertReadEnabled();
	AcGePoint3d ptC, pti;
	double cx = 0.0, cy = 0.0, cz = 0.0;

	for (uint i = 0; i < numVerts(); i++)
	{
		this->getPointAt(i, pti);
		cx += pti[X];
		cy += pti[Y];
		cz += pti[Z];
	}

	cx = cx / numVerts();
	cy = cy / numVerts();
	cz = cz / numVerts();

	ptC.set(cx, cy, cz);
	return ptC;
}

//-----------------------------------------------------------------------------
//----- AcDbCurve protocols
//- Curve property tests.
Adesk::Boolean AuPolyline::isClosed() const
{
	assertReadEnabled();
	return (AcDbPolyline::isClosed());
}

Adesk::Boolean AuPolyline::isPeriodic() const
{
	assertReadEnabled();
	return (AcDbPolyline::isPeriodic());
}

//- Get planar and start/end geometric properties.
Acad::ErrorStatus AuPolyline::getStartParam(double& param) const
{
	assertReadEnabled();
	return (AcDbPolyline::getStartParam(param));
}

Acad::ErrorStatus AuPolyline::getEndParam(double& param) const
{
	assertReadEnabled();
	return (AcDbPolyline::getEndParam(param));
}

Acad::ErrorStatus AuPolyline::getStartPoint(AcGePoint3d & point) const
{
	assertReadEnabled();
	return (AcDbPolyline::getStartPoint(point));
}

Acad::ErrorStatus AuPolyline::getEndPoint(AcGePoint3d & point) const
{
	assertReadEnabled();
	return (AcDbPolyline::getEndPoint(point));
}

//- Conversions to/from parametric/world/distance.
Acad::ErrorStatus AuPolyline::getPointAtParam(double param, AcGePoint3d & point) const
{
	assertReadEnabled();
	return (AcDbPolyline::getPointAtParam(param, point));
}

Acad::ErrorStatus AuPolyline::getParamAtPoint(const AcGePoint3d & point, double& param) const
{
	assertReadEnabled();
	return (AcDbPolyline::getParamAtPoint(point, param));
}

Acad::ErrorStatus AuPolyline::getDistAtParam(double param, double& dist) const
{
	assertReadEnabled();
	return (AcDbPolyline::getDistAtParam(param, dist));
}

Acad::ErrorStatus AuPolyline::getParamAtDist(double dist, double& param) const
{
	assertReadEnabled();
	return (AcDbPolyline::getParamAtDist(dist, param));
}

Acad::ErrorStatus AuPolyline::getDistAtPoint(const AcGePoint3d & point, double& dist) const
{
	assertReadEnabled();
	return (AcDbPolyline::getDistAtPoint(point, dist));
}

Acad::ErrorStatus AuPolyline::getPointAtDist(double dist, AcGePoint3d & point) const
{
	assertReadEnabled();
	return (AcDbPolyline::getPointAtDist(dist, point));
}

//- Derivative information.
Acad::ErrorStatus AuPolyline::getFirstDeriv(double param, AcGeVector3d & firstDeriv) const
{
	assertReadEnabled();
	return (AcDbPolyline::getFirstDeriv(param, firstDeriv));
}

Acad::ErrorStatus AuPolyline::getFirstDeriv(const AcGePoint3d & point, AcGeVector3d & firstDeriv) const
{
	assertReadEnabled();
	return (AcDbPolyline::getFirstDeriv(point, firstDeriv));
}

Acad::ErrorStatus AuPolyline::getSecondDeriv(double param, AcGeVector3d & secDeriv) const
{
	assertReadEnabled();
	return (AcDbPolyline::getSecondDeriv(param, secDeriv));
}

Acad::ErrorStatus AuPolyline::getSecondDeriv(const AcGePoint3d & point, AcGeVector3d & secDeriv) const
{
	assertReadEnabled();
	return (AcDbPolyline::getSecondDeriv(point, secDeriv));
}

//- Closest point on curve.
Acad::ErrorStatus AuPolyline::getClosestPointTo(const AcGePoint3d & givenPnt, AcGePoint3d & pointOnCurve, Adesk::Boolean extend /*=Adesk::kFalse*/) const
{
	assertReadEnabled();
	return (AcDbPolyline::getClosestPointTo(givenPnt, pointOnCurve, extend));
}

Acad::ErrorStatus AuPolyline::getClosestPointTo(const AcGePoint3d & givenPnt, const AcGeVector3d & direction, AcGePoint3d & pointOnCurve, Adesk::Boolean extend /*=Adesk::kFalse*/) const
{
	assertReadEnabled();
	return (AcDbPolyline::getClosestPointTo(givenPnt, direction, pointOnCurve, extend));
}

//- Get a projected copy of the curve.
Acad::ErrorStatus AuPolyline::getOrthoProjectedCurve(const AcGePlane & plane, AcDbCurve * &projCrv) const
{
	assertReadEnabled();
	return (AcDbPolyline::getOrthoProjectedCurve(plane, projCrv));
}

Acad::ErrorStatus AuPolyline::getProjectedCurve(const AcGePlane & plane, const AcGeVector3d & projDir, AcDbCurve * &projCrv) const
{
	assertReadEnabled();
	return (AcDbPolyline::getProjectedCurve(plane, projDir, projCrv));
}

//- Get offset, spline and split copies of the curve.
Acad::ErrorStatus AuPolyline::getOffsetCurves(double offsetDist, AcDbVoidPtrArray & offsetCurves) const
{
	assertReadEnabled();
	return (AcDbPolyline::getOffsetCurves(offsetDist, offsetCurves));
}

Acad::ErrorStatus AuPolyline::getOffsetCurvesGivenPlaneNormal(const AcGeVector3d & normal, double offsetDist, AcDbVoidPtrArray & offsetCurves) const
{
	assertReadEnabled();
	return (AcDbPolyline::getOffsetCurvesGivenPlaneNormal(normal, offsetDist, offsetCurves));
}

Acad::ErrorStatus AuPolyline::getSpline(AcDbSpline * &spline) const
{
	assertReadEnabled();
	return (AcDbPolyline::getSpline(spline));
}

Acad::ErrorStatus AuPolyline::getSplitCurves(const AcGeDoubleArray & params, AcDbVoidPtrArray & curveSegments) const
{
	assertReadEnabled();
	return (AcDbPolyline::getSplitCurves(params, curveSegments));
}

Acad::ErrorStatus AuPolyline::getSplitCurves(const AcGePoint3dArray & points, AcDbVoidPtrArray & curveSegments) const
{
	assertReadEnabled();
	return (AcDbPolyline::getSplitCurves(points, curveSegments));
}

//- Extend the curve.
Acad::ErrorStatus AuPolyline::extend(double newParam)
{
	assertReadEnabled();
	return (AcDbPolyline::extend(newParam));
}

Acad::ErrorStatus AuPolyline::extend(Adesk::Boolean extendStart, const AcGePoint3d & toPoint)
{
	assertReadEnabled();
	return (AcDbPolyline::extend(extendStart, toPoint));
}

//- Area calculation.
Acad::ErrorStatus AuPolyline::getArea(double& area) const
{
	assertReadEnabled();
	return (AcDbPolyline::getArea(area));
}

