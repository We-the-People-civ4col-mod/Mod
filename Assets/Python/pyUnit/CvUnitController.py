from CvPythonExtensions import *
import random

MoveFormation_BoxRanges = [ [-0.8, -0.8, 0.8, 0.8],    # Two
							[-0.8, -0.8, -0.1, 0.8 ,  0.1, -0.8, 0.8, 0.8],    # Three
							[-0.8, -0.8, -0.1, -0.1,  0.1, -0.8, 0.8, -0.1,  -0.5, 0.1, 0.5, 0.8],    # Four
							[-0.8, -0.8, -0.2, -0.2,  -0.8, 0.2, -0.2, 0.8,  0.2, -0.8, 0.8, -0.2,   0.2, 0.2, 0.8, 0.8], # Five
							[-0.8, -0.8, -0.2, -0.2,  -0.8, 0.2, -0.2, 0.8,  0.2, -0.8, 0.8, -0.2,   0.2, 0.2, 0.8, 0.8, -0.3, -0.3, 0.3, 0.3]]
random.seed( 37 )   # should this be configurable?

class CvUnitController:

	def __init__(self):
		pass						# One


	def ProcessEvent ( self, pCyUnitEntity, iEvent):
		"""Controls the logic to react to the event."""

		if iEvent == EntityEventTypes.ENTEVENT_MOVE:

			iNumAlive = pCyUnitEntity.GetUnitsCurrentlyAlive()
			if iNumAlive > 5:   # We can't handle more than 5 sub-units...
				return 0;
			iCurrentUnit = 0
			for i in range(0, pCyUnitEntity.GetSubEntityCount()):

				#
				pCyUnitSubEntity = pCyUnitEntity.GetSubEntity(i);
				if pCyUnitSubEntity.isDead():
					continue
				x = random.uniform( MoveFormation_BoxRange[iNumAlive][iCurrentUnit * 4 + 0], MoveFormation_BoxRange[iNumAlive][iCurrentUnit * 4 + 2] )
				y = random.uniform( MoveFormation_BoxRange[iNumAlive][iCurrentUnit * 4 + 1], MoveFormation_BoxRange[iNumAlive][iCurrentUnit * 4 + 3] )
				kTarget = pCyUnitEntity.TranslateFormation( x, y )
				pCyUnitSubEntity.SetPositionTarget( kTarget.x, kTarget.y, 0.0 )
				pCyUnitSubEntity.SetAnimationPath( AnimationPathTypes.ANIMATIONPATH_RUN )
				pCyUnitSubEntity.SetStoppingCriteria( BehaviourStoppingCriterion.STOPCRIT_DESTINATION )
				pCyUnitSubEntity.SetFacingTargetTowardsPositionTarget()
				pCyUnitSubEntity.PushBehaviourState( )
				iCurrentUnit += 1;

			return 1;

		else:
			return 0;

