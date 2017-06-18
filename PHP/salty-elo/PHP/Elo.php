<?php

/**
 * This class calculates rankings using the FIDE Elo system.
 *
 * @author Kevin Zhang <kzhaaang@gmail.com>
 * @copyright Copyright (c) 2015 Kevin Zhang
 * @license MIT License
 */

require 'Player.php';

class Elo {

  protected $playerA;
  protected $playerB;

  /**
   * constructor function
   * $pA  = Player A
   * $pB  = Player B
   */
  public function __construct($pA, $pB) {
    $this->playerA = $pA;
    $this->playerB = $pB;
  }

  /**
   * rank two players based on results
   * score of 1 = win, 0 = loss (both 0.5 being a tie)
   * note: anything else will give strange results
   */
  public function rank($scoreA, $scoreB) {
    $expectedA = Elo::expectedScore($this->playerA->getElo(), $this->playerB->getElo());
    $expectedB = Elo::expectedScore($this->playerB->getElo(), $this->playerA->getElo());
    Elo::calculateElo($this->playerA, $scoreA, $expectedA);
    Elo::calculateElo($this->playerB, $scoreB, $expectedB);
    $this->playerA->playGames();
    $this->playerB->playGames();
  }

  /**
   * rank player after rating period / tournament
   * $scores should be an array of their wins/losses/draws in order
   * $rankings should be an array of their opponent's elo prior to the
   * rating period / tournament
   * NOTE: rankings done this way are computed all at once; players in
   * provisionals prior to the rating period / tournament will be
   * calculated entirely as a provisional player
   * WARNING: if $scores and $rankings do not hold the same number of
   * elements, rankings WILL get strange - check for it prior to
   * calling the function and handle the error gracefully yourself
   */
  public static function rankAll($player, $scores, $rankings) {
    $totalScores = array_sum($scores);
    // get the expected score for each ranking
    $expectedScores = array();
    foreach($rankings as $ranking) {
      array_push($expectedScores, Elo::expectedScore($player->getElo(), $ranking));
    }
    $totalExpected = array_sum($expectedScores);
    Elo::calculateElo($player, $totalScores, $totalExpected);
    $player->playGames(count($scores));
  }

  /**
   * calculate expected score
   * $rankingA  = ranking of the player
   * $rankingB  = ranking of the opponent
   * returns expected score of the player
   */
  public static function expectedScore($rankingA, $rankingB) {
    $expectedScore = $rankingB - $rankingA;
    return 1 / (1 + (pow(10, $expectedScore / 400)));
  }

  /**
   * calculate new elo
   * $p   = Player object
   * $kf  = k-factor
   */
  public static function newElo($p, $kf, $score, $expectedScore) {
    $newElo = $p->getElo() + ($kf * ($score - $expectedScore));
    $p->setElo($newElo);
  }

  /**
   * scale elo appropriately
   * override this method for different k-factor scaling
   * k-factor scaling as follows:
   * k=40 for new players (still in provisionals)
   * k=20 for players who have never reached 2400
   * k=10 for players who have reached 2400 and are not in provisionals
   */
  public static function calculateElo($p, $score, $expectedScore) {
    if($p->inProvisionals()) {
      Elo::newElo($p, 40, $score, $expectedScore);
    }
    else if($p->highestElo() < 2400) {
      Elo::newElo($p, 20, $score, $expectedScore);
    }
    else {
      Elo::newElo($p, 10, $score, $expectedScore);
    }
  }

}

?>
